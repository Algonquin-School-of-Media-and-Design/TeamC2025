/************************************************************************************
 * Author:   Tristan Anglin
 * Year:     2025
 *
 * Notice:
 *   This source code is the intellectual property of Tristan Anglin.
 *   It is provided solely for use within:
 *     - The Relic Runners personal project
 *     - The Almost A Dozen project
 *
 *   Any use, distribution, or modification outside of these projects
 *   is strictly prohibited without explicit written permission.
 *
 *   © 2025 Tristan Anglin. All rights reserved.
 ************************************************************************************/

#include "RelicRunnersPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "RelicRunners/RelicRunnersCharacter.h"
#include "RelicRunners/Inventory/Inventory.h"
#include "InputActionValue.h"
#include "RelicRunners/Item/ItemActor.h"
#include "Net/UnrealNetwork.h"
#include "RelicRunners/PlayerPreview/PlayerPreview.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "RelicRunners/PlayerState/RelicRunnersPlayerState.h"
#include "RelicRunners/Inventory/InventoryComponent.h"
#include <RelicRunners/Item/ItemStats.h>
#include <Kismet/GameplayStatics.h>
#include <RelicRunners/Game/RelicRunnersGameInstance.h>
#include "RelicRunners/Menu/MainMenuWidget.h"

ARelicRunnersPlayerController::ARelicRunnersPlayerController()
{

}

void ARelicRunnersPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARelicRunnersPlayerController, PlayerPreviewInstance);
}


void ARelicRunnersPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	PossessedPawn = Cast<ARelicRunnersCharacter>(aPawn);
}

void ARelicRunnersPlayerController::Server_SetPlayerName_Implementation(const FString& NewName)
{
	ARelicRunnersPlayerState* PS = GetPlayerState<ARelicRunnersPlayerState>();
	if (PS)
	{
		PS->SetPlayerName(NewName);
	}
}

void ARelicRunnersPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	if (IsLocalController())
	{
		PossessedPawn = Cast<ARelicRunnersCharacter>(GetPawn());
		UE_LOG(LogTemp, Warning, TEXT("OnRep_Pawn: LocalController detected"));
	}
}

void ARelicRunnersPlayerController::OnRep_PlayerPreview()
{
	if (IsLocalController())
	{
		UE_LOG(LogTemp, Warning, TEXT("OnRep_PlayerPreviewInstance: Preview instance replicated"));

		if (PlayerPreviewInstance)
		{
			SetupPreviewRenderTarget(PlayerPreviewInstance);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("OnRep_PlayerPreviewInstance: Still null"));
		}
	}
}

void ARelicRunnersPlayerController::UpdatePreviewWithEquippedItems()
{
	if (!PlayerPreviewInstance || !GetPawn()) return;

	ARelicRunnersCharacter* Char = Cast<ARelicRunnersCharacter>(GetPawn());
	if (!Char) return;

	// Clear all visuals on preview first
	PlayerPreviewInstance->ClearAllPreviewMeshes();

	const TArray<FEquippedItemEntry>& EquippedEntries = Char->GetInventoryComponent()->EquippedItems;

	for (const FEquippedItemEntry& Entry : EquippedEntries)
	{
		if (!Entry.Item) continue;

		const FString& ItemType = Entry.ItemType;
		const FString MeshPath = Char->GetMeshPathByItemType(ItemType);

		UObject* LoadedMesh = nullptr;
		if (ItemType == "Sword" || ItemType == "Shield")
		{
			LoadedMesh = StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *MeshPath);
		}
		else
		{
			LoadedMesh = StaticLoadObject(USkeletalMesh::StaticClass(), nullptr, *MeshPath);
		}

		if (!LoadedMesh) continue;

		UpdatePreviewItemVisual(LoadedMesh, ItemType);
	}
}

void ARelicRunnersPlayerController::UpdatePreviewItemVisual(UObject* MeshAsset, const FString& ItemType)
{
	if (!PlayerPreviewInstance) return;

	// Set or clear the mesh on the preview actor based on asset type
	if (ItemType == "Sword" || ItemType == "Shield")
	{
		PlayerPreviewInstance->SetPreviewMeshByItemType(ItemType, Cast<UStaticMesh>(MeshAsset));
	}
	else
	{
		PlayerPreviewInstance->SetPreviewMeshByItemType(ItemType, Cast<USkeletalMesh>(MeshAsset));
	}
}

void ARelicRunnersPlayerController::Server_RequestPickup_Implementation(AItemActor* Item)
{
	if (Item)
	{
		Item->HandlePickup(Cast<ARelicRunnersCharacter>(GetPawn()));
	}
}

void ARelicRunnersPlayerController::OnUnPossess()
{
	APawn* MyPawn = Cast<APawn>(GetOwner());

	Super::OnUnPossess();
}

void ARelicRunnersPlayerController::SetupPreviewRenderTarget(APlayerPreview* Preview)
{
	if (!IsLocalController() || !Preview)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SetupPreviewRenderTarget] Invalid call or not local controller."));
		return;
	}

	ARelicRunnersCharacter* Char = Cast<ARelicRunnersCharacter>(GetPawn());
	if (!Char)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SetupPreviewRenderTarget] Missing character."));
		return;
	}

	// If inventory isn't ready, try again shortly
	if (!Char->Inventory)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SetupPreviewRenderTarget] Inventory is null. Retrying in 0.2s..."));

		FTimerDelegate RetryDelegate;
		RetryDelegate.BindUFunction(this, FName("SetupPreviewRenderTarget"), Preview);

		FTimerHandle RetryHandle;
		GetWorld()->GetTimerManager().SetTimer(RetryHandle, RetryDelegate, 0.2f, false);
		return;
	}

	// Assign the render target image to the inventory UI
	UTextureRenderTarget2D* RenderTarget = Preview->CreateRenderTarget();
	if (RenderTarget)
	{
		Char->Inventory->SetPreviewActorImage(RenderTarget);
		UE_LOG(LogTemp, Warning, TEXT("[SetupPreviewRenderTarget] Applied render target to UI."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SetupPreviewRenderTarget] Failed to create render target."));
	}
}

void ARelicRunnersPlayerController::TrySetupPreviewRenderTarget()
{
	if (!IsLocalController())
		return;

	if (!PlayerPreviewInstance)
	{
		// Retry in a moment if not replicated yet
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ARelicRunnersPlayerController::TrySetupPreviewRenderTarget);
		return;
	}

	SetupPreviewRenderTarget(PlayerPreviewInstance);
}

void ARelicRunnersPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		// Input Mapping
		if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}

		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, [this]() { ARelicRunnersCharacter* Char = Cast<ARelicRunnersCharacter>(GetPawn());
			if (!Char) return;

			if (UInventoryComponent* Inv = Char->GetInventoryComponent())
			{
			
				Inv->OnEquipmentChanged.AddUniqueDynamic(this, &ARelicRunnersPlayerController::UpdatePreviewWithEquippedItems);
			}
		}, 0.5f, false);

		if (URelicRunnersGameInstance* GI = GetGameInstance<URelicRunnersGameInstance>())
		{
			Server_SetPlayerName(GI->GetCharacterName());
		}

		FString MapName = GetWorld()->GetMapName();
		MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

		if (MapName.Contains("MainMenu"))
		{
			UWorld* World = GetWorld();
			if (World)
			{
				TArray<AActor*> FoundActors;
				UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundActors);
				AActor* MainMenuCamera = nullptr;

				for (auto actor : FoundActors)
				{
					if (actor->ActorHasTag("MainMenuCamera"))
					{
						MainMenuCamera = actor;
					}
				}

				if (MainMenuCamera)
				{
					SetViewTargetWithBlend(MainMenuCamera, 0.0f);
				}
			}

			if (MainMenuWidgetClass && !MainMenuWidget)
			{
				MainMenuWidget = CreateWidget<UMainMenuWidget>(this, MainMenuWidgetClass);
				if (MainMenuWidget)
				{
					MainMenuWidget->AddToViewport();
					MainMenuWidget->SetIsEnabled(true);
					MainMenuWidget->SetVisibility(ESlateVisibility::Visible);
					SetShowMouseCursor(true);
					SetInputMode(FInputModeGameAndUI());
				}
			}
		}
		else if (MapName.Contains("Lobby"))
		{
			
		}
	}
}

void ARelicRunnersPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ARelicRunnersPlayerController::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ARelicRunnersPlayerController::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Triggered, this, &ARelicRunnersPlayerController::Walk);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &ARelicRunnersPlayerController::Run);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARelicRunnersPlayerController::Look);

		// Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ARelicRunnersPlayerController::Interact);

		// Inventory
		EnhancedInputComponent->BindAction(InventoryAction, ETriggerEvent::Started, this, &ARelicRunnersPlayerController::InventoryUI);

		// BasicAttack
		EnhancedInputComponent->BindAction(BasicAttackAction, ETriggerEvent::Started, this, &ARelicRunnersPlayerController::BasicAttack);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ARelicRunnersPlayerController::Walk(const FInputActionValue& Value)
{
	if (PossessedPawn)
	{
		PossessedPawn->Walk(Value);
	}
}

void ARelicRunnersPlayerController::Run(const FInputActionValue& Value)
{
	if (PossessedPawn)
	{
		PossessedPawn->Run(Value);
	}
}

void ARelicRunnersPlayerController::Look(const FInputActionValue& Value)
{
	if (PossessedPawn)
	{
		PossessedPawn->Look(Value);
	}
}

void ARelicRunnersPlayerController::Interact()
{
	if (PossessedPawn)
	{
		PossessedPawn->Interact();
	}
}

void ARelicRunnersPlayerController::InventoryUI()
{
	if (PossessedPawn)
	{
		PossessedPawn->InventoryUI();
	}
}

void ARelicRunnersPlayerController::BasicAttack()
{
	if (PossessedPawn)
	{
		PossessedPawn->BasicAttack();
	}
}

void ARelicRunnersPlayerController::Jump(const FInputActionValue& Value)
{
	if (PossessedPawn)
	{
		PossessedPawn->Jump();
	}
}

void ARelicRunnersPlayerController::StopJumping(const FInputActionValue& Value)
{
	if (PossessedPawn)
	{
		PossessedPawn->StopJumping();
	}
}