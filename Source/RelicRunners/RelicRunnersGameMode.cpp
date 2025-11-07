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
#include "Game/RelicRunnersGameInstance.h"

ARelicRunnersGameMode::ARelicRunnersGameMode()
{
	// Set default pawn class and player controller
	static ConstructorHelpers::FClassFinder<APlayerPreview> PlayerPreviewBPClass(TEXT("/Game/ThirdPerson/Blueprints/PlayerPreview/BP_PlayerPreview"));

	if (PlayerPreviewBPClass.Class)
	{
		PlayerPreviewClass = PlayerPreviewBPClass.Class;
		UE_LOG(LogTemp, Warning, TEXT("[GameMode] Assigned PlayerPreviewClass from constructor."));
	}

    bUseSeamlessTravel = true;
    bReplicates = true;
}

void ARelicRunnersGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    static int32 PlayerIndex = 1;
    if (ARelicRunnersPlayerState* PS = NewPlayer->GetPlayerState<ARelicRunnersPlayerState>())
    {
        PS->SetPlayerName("Player" + FString::FromInt(PlayerIndex++));
    }

    // Skip the host player (listen server’s own controller)
    if (NewPlayer->IsLocalController())
    {
        UE_LOG(LogTemp, Warning, TEXT("[GameMode] Skipping preview spawn for host: %s"), *NewPlayer->GetName());
        return;
    }

    if (ARelicRunnersPlayerController* RRPC = Cast<ARelicRunnersPlayerController>(NewPlayer))
    {
        const FVector SpawnLocation = FVector(-1000000 * PlayerIndex, 0, -1000);
        const FRotator SpawnRotation = FRotator::ZeroRotator;

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = NewPlayer;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        if (PlayerPreviewClass && !RRPC->PlayerPreviewInstance)
        {
            APlayerPreview* Preview = GetWorld()->SpawnActor<APlayerPreview>(PlayerPreviewClass, SpawnLocation, SpawnRotation, SpawnParams);
            if (Preview)
            {
                RRPC->PlayerPreviewInstance = Preview;
                RRPC->TrySetupPreviewRenderTarget();
                UE_LOG(LogTemp, Warning, TEXT("[GameMode] Spawned preview for %s: %s"), *RRPC->GetName(), *Preview->GetName());
            }
        }
    }
}

void ARelicRunnersGameMode::PostSeamlessTravel()
{
    Super::PostSeamlessTravel();

    UE_LOG(LogTemp, Warning, TEXT("[PostSeamlessTravel] Re-initializing players after seamless travel"));

    static int32 PlayerIndex = 1;

    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PC = It->Get();
        if (!PC) continue;

        // Make sure the player has a pawn
        if (!PC->GetPawn())
        {
            RestartPlayer(PC);
        }

        // Optional: reset name if needed
        if (ARelicRunnersPlayerState* PS = PC->GetPlayerState<ARelicRunnersPlayerState>())
        {
            if (PS->GetPlayerName().IsEmpty())
            {
                PS->SetPlayerName(FString::Printf(TEXT("Player%d"), PlayerIndex++));
            }
        }

        // Spawn preview actor (same as before)
        if (PlayerPreviewClass)
        {
            const FVector SpawnLocation = FVector(-1000000 * PlayerIndex, 0, -1000);
            const FRotator SpawnRotation = FRotator::ZeroRotator;

            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = PC;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

            APlayerPreview* Preview = GetWorld()->SpawnActor<APlayerPreview>(PlayerPreviewClass, SpawnLocation, SpawnRotation, SpawnParams);
            if (Preview)
            {
                Preview->SetOwner(PC);

                if (ARelicRunnersPlayerController* RRPC = Cast<ARelicRunnersPlayerController>(PC))
                {
                    RRPC->PlayerPreviewInstance = Preview;
                    RRPC->TrySetupPreviewRenderTarget();
                }
            }
        }
    }
}

bool ARelicRunnersGameMode::InitializeTriggerState()
{
    switch (ObjectiveType)
    {
    case EObjectiveType::None:
        return true;
        break;
    case EObjectiveType::CaptureTheFlag:
        return false;
        break;
    }
    return false;
}

void ARelicRunnersGameMode::Multicast_SetObjectiveType_Implementation(EObjectiveType newType)
{
    ObjectiveType = newType;

    switch (ObjectiveType)
    {
    case EObjectiveType::None:
        Multicast_DecrementObjective();
        break;
    }

}

void ARelicRunnersGameMode::Multicast_IncrementObjective_Implementation()
{
    RemainingObjectives++;
    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("Max objectives is now: %i"), RemainingObjectives));

}

void ARelicRunnersGameMode::Multicast_DecrementObjective_Implementation()
{
    RemainingObjectives--;

    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("Remaining objectives: %i"), RemainingObjectives));

    if (RemainingObjectives <= 0)
    {
        OnObjectiveActionCompleted.Broadcast();
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("Objective has been reached!")));
    }
}
