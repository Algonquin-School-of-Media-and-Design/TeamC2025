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

#include "RelicRunnersGameMode.h"
#include "RelicRunnersCharacter.h"
#include "PlayerState/RelicRunnersPlayerState.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "PlayerPreview/PlayerPreview.h"
#include "PlayerController/RelicRunnersPlayerController.h"

ARelicRunnersGameMode::ARelicRunnersGameMode()
{
	// Set default pawn class and player controller
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_Player"));
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_RelicRunnersPlayerController"));
	static ConstructorHelpers::FClassFinder<APlayerPreview> PlayerPreviewBPClass(TEXT("/Game/ThirdPerson/Blueprints/PlayerPreview/BP_PlayerPreview"));

	if (PlayerPawnBPClass.Class)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
		UE_LOG(LogTemp, Warning, TEXT("[GameMode] Assigned PlayerPawnClass from constructor."));
	}

	if (PlayerControllerBPClass.Class)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
		UE_LOG(LogTemp, Warning, TEXT("[GameMode] Assigned PlayerControllerClass from constructor."));
	}

	if (PlayerPreviewBPClass.Class)
	{
		PlayerPreviewClass = PlayerPreviewBPClass.Class;
		UE_LOG(LogTemp, Warning, TEXT("[GameMode] Assigned PlayerPreviewClass from constructor."));
	}

	PlayerStateClass = ARelicRunnersPlayerState::StaticClass();
}

void ARelicRunnersGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// Give unique player name
	static int32 PlayerIndex = 1;
	if (ARelicRunnersPlayerState* PS = Cast<ARelicRunnersPlayerState>(NewPlayer->PlayerState))
	{
		FString NewName = FString::Printf(TEXT("Player%d"), PlayerIndex++);
		PS->SetPlayerName(NewName);
		UE_LOG(LogTemp, Warning, TEXT("PostLogin: Assigned name %s to player controller %s"), *NewName, *NewPlayer->GetName());
	}

	// Log pawn info (may be nullptr if not spawned yet)
	if (APawn* SpawnedPawn = NewPlayer->GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("PostLogin: %s is possessing %s"), *NewPlayer->GetName(), *SpawnedPawn->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PostLogin: %s does not yet have a pawn (will be spawned later)"), *NewPlayer->GetName());
	}

	// === SPAWN THE PREVIEW ACTOR ON THE SERVER ===
	// Spawn the preview very far from other previews and players
	const FVector SpawnLocation = FVector(-1000000 * PlayerIndex, 0, -1000); // Off-screen far away
	const FRotator SpawnRotation = FRotator::ZeroRotator;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = NewPlayer;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	APlayerPreview* Preview = GetWorld()->SpawnActor<APlayerPreview>(PlayerPreviewClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (Preview)
	{
		Preview->SetOwner(NewPlayer);

		// Use your custom setter so that replication to clients is handled correctly
		if (ARelicRunnersPlayerController* RRPC = Cast<ARelicRunnersPlayerController>(NewPlayer))
		{
			RRPC->PlayerPreviewInstance = Preview;
			RRPC->TrySetupPreviewRenderTarget();
			UE_LOG(LogTemp, Warning, TEXT("[GameMode] Spawned preview actor for %s: %s"), *NewPlayer->GetName(), *Preview->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[GameMode] Could not cast %s to ARelicRunnersPlayerController"), *NewPlayer->GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[GameMode] Failed to spawn preview actor for %s"), *NewPlayer->GetName());
	}
}