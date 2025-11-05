// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingsWidget.h"
#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/TileView.h"
#include "KeybindingsListData.h"
#include <InputMappingContext.h>
#include <EnhancedInputSubsystems.h>
#include "Keybinds.h"
#include "KeybindingsListWidget.h"
#include <RelicRunners/PlayerController/RelicRunnersPlayerController.h>
#include <Kismet/GameplayStatics.h>

void USettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &USettingsWidget::BackButtonClicked);
	}

	if (B_RestoreDefaults)
	{
		B_RestoreDefaults->OnClicked.AddDynamic(this, &USettingsWidget::OnRestoreDefaultsClicked);
	}
}

void USettingsWidget::OnRestoreDefaultsClicked()
{
	
}

void USettingsWidget::StartRebinding(UKeybindingsListWidget* Entry)
{
	WaitingForKeyEntry = Entry;
}

FReply USettingsWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (!WaitingForKeyEntry)
		return Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	FKey PressedKey = InKeyEvent.GetKey();
	if (!PressedKey.IsValid())
		return FReply::Handled();

	return HandleKeyBindPressed(PressedKey);
}

FReply USettingsWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!WaitingForKeyEntry)
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	// Use the mouse button that was pressed
	FKey PressedKey = InMouseEvent.GetEffectingButton();
	if (!PressedKey.IsValid())
		return FReply::Handled();

	return HandleKeyBindPressed(PressedKey);
}

FReply USettingsWidget::HandleKeyBindPressed(FKey PressedKey)
{
	// Get the data object for the row we’re rebinding
	UKeybindingsListData* Data = WaitingForKeyEntry->GetKeybindData();
	if (!Data)
		return FReply::Handled();

	// First, find all other bindings that use this key
	for (UKeybindingsListData* Binding : DefaultKeybindings)
	{
		if (Binding != Data && Binding->Keybind == PressedKey)
		{
			Binding->Keybind = EKeys::Invalid;
		}
	}

	// Assign new key
	Data->Keybind = PressedKey;

	// Update PlayerController keybinds
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (ARelicRunnersPlayerController* RPC = Cast<ARelicRunnersPlayerController>(PC))
		{
			if (UKeybinds* Keys = RPC->GetKeybinds())
			{
				for (auto& Bind : Keys->KeyBinds)
				{
					if (Bind.Name != Data->Name && Bind.Bind == PressedKey)
						Bind.Bind = EKeys::Invalid;

					if (Bind.Name == Data->Name)
						Bind.Bind = PressedKey;
				}
			}
		}
	}

	// Apply changes to input system
	ApplyKeybindings();

	// Refresh all visuals
	for (UKeybindingsListData* Binding : DefaultKeybindings)
	{
		if (Binding->BoundWidget)
		{
			Binding->BoundWidget->RefreshVisual();
		}
	}

	// Stop listening
	WaitingForKeyEntry = nullptr;

	return FReply::Handled();
}

void USettingsWidget::OnTileViewScrolled()
{
	// Refresh all currently active widgets
	for (UKeybindingsListData* Binding : DefaultKeybindings)
	{
		if (Binding->BoundWidget)
		{
			Binding->BoundWidget->RefreshVisual();
		}
	}
}

void USettingsWidget::SetParentMenu(UMainMenuWidget* InParentMenu)
{
	ParentMenu = InParentMenu;
}

void USettingsWidget::BackButtonClicked()
{
	if (ParentMenu)
	{
		ParentMenu->ShowModeSelectionWidget();
	}
}

void USettingsWidget::InitializeDefaultKeybindings()
{
	DefaultKeybindings.Empty();

	auto AddBinding = [&](const FString& ActionName, const FKey& Key)
		{
			UKeybindingsListData* NewEntry = NewObject<UKeybindingsListData>(this);
			NewEntry->Name = ActionName;
			NewEntry->Keybind = Key;
			DefaultKeybindings.Add(NewEntry);
		};

	ARelicRunnersPlayerController* RPC = Cast<ARelicRunnersPlayerController>(GetOwningPlayer());
	if (!RPC) return;

	for (auto Binds : RPC->GetKeybinds()->KeyBinds)
	{
		AddBinding(Binds.Name, Binds.Bind);
	}

	if (KeybindingsTileView)
	{
		KeybindingsTileView->ClearListItems();

		for (UKeybindingsListData* Binding : DefaultKeybindings)
		{
			KeybindingsTileView->AddItem(Binding);
		}
	}

	ApplyKeybindings();
}

void USettingsWidget::ApplyKeybindings()
{

	// Get the player input subsystem
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	ARelicRunnersPlayerController* RPC = Cast<ARelicRunnersPlayerController>(PC);
	if (!RPC) return;

	UInputMappingContext* MappingContext = RPC->GetInputMappingContext();
	if (!MappingContext)
	{
		UE_LOG(LogTemp, Warning, TEXT("ApplyKeybindings: No InputMappingContext set!"));
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	if (!Subsystem) return;

	MappingContext->UnmapAll();

	// Restore Look
	UInputAction* LookAction = LoadObject<UInputAction>(
		nullptr,
		TEXT("/Game/ThirdPerson/Input/Actions/IA_Look.IA_Look")
	);

	if (LookAction)
	{
		FEnhancedActionKeyMapping& Mapping = MappingContext->MapKey(LookAction, EKeys::Mouse2D);
		
		UE_LOG(LogTemp, Log, TEXT("ApplyKeybindings: Look (Mouse2D)"));

		UInputModifierNegate* NegateModifier = NewObject<UInputModifierNegate>();
		NegateModifier->bX = RPC->GetKeybinds()->InvertedXMouse; // Invert X
		NegateModifier->bY = RPC->GetKeybinds()->InvertedYMouse; // Invert Y
		NegateModifier->bZ = false;

		Mapping.Modifiers.Add(NegateModifier);
		UE_LOG(LogTemp, Log, TEXT("Look Modifiers: X | %s Y | %s"),
			RPC->GetKeybinds()->InvertedXMouse ? TEXT("true") : TEXT("false"),
			RPC->GetKeybinds()->InvertedYMouse ? TEXT("true") : TEXT("false")
		);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ApplyKeybindings: couldn't find IA_Look asset!"));
	}

	for (UKeybindingsListData* Binding : DefaultKeybindings)
	{
		if (!Binding) continue;

		// Build action asset name (your naming convention)
		FString ActionAssetName = FString::Printf(TEXT("IA_%s"), *Binding->Name.Replace(TEXT(" "), TEXT("")));
		FString AssetPath = FString::Printf(TEXT("/Game/ThirdPerson/Input/Actions/%s.%s"), *ActionAssetName, *ActionAssetName);

		UInputAction* FoundAction = LoadObject<UInputAction>(nullptr, *AssetPath);
		if (!FoundAction)
		{
			UE_LOG(LogTemp, Warning, TEXT("ApplyKeybindings: couldn't load InputAction %s (path: %s)"), *ActionAssetName, *AssetPath);
			continue;
		}

		// Map key for the action on the mapping context
		// MapKey usually exists as a convenience wrapper exposed in the API you used earlier
		MappingContext->MapKey(FoundAction, Binding->Keybind);

		UE_LOG(LogTemp, Log, TEXT("ApplyKeybindings: mapped action %s -> key %s"), *Binding->Name, *Binding->Keybind.GetFName().ToString());
	}

	// Re-apply mapping context to the subsystem (remove old first if necessary)
	// You may want to remove then add to ensure updated mappings are used:
	Subsystem->RemoveMappingContext(MappingContext);
	Subsystem->AddMappingContext(MappingContext, 0);
}

void USettingsWidget::ToggleInvertedXMouse()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	ARelicRunnersPlayerController* RPC = Cast<ARelicRunnersPlayerController>(PC);
	if (!RPC) return;

	if (RPC->GetKeybinds()->InvertedXMouse) { RPC->GetKeybinds()->InvertedXMouse = false; }
	else { RPC->GetKeybinds()->InvertedXMouse = true; }
}

void USettingsWidget::ToggleInvertedYMouse()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	ARelicRunnersPlayerController* RPC = Cast<ARelicRunnersPlayerController>(PC);
	if (!RPC) return;

	if (RPC->GetKeybinds()->InvertedYMouse) { RPC->GetKeybinds()->InvertedYMouse = false; }
	else { RPC->GetKeybinds()->InvertedYMouse = true; }
}
