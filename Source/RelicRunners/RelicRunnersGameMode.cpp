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
}

void ARelicRunnersGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    //fallback name
    static int32 PlayerIndex = 1;
    if (ARelicRunnersPlayerState* PS = NewPlayer->GetPlayerState<ARelicRunnersPlayerState>())
    {
        PS->SetPlayerName("Player" + FString::FromInt(PlayerIndex++));
    }

    // === SPAWN THE PREVIEW ACTOR (OFFSCREEN) ===
    const FVector SpawnLocation = FVector(-1000000 * PlayerIndex, 0, -1000);
    const FRotator SpawnRotation = FRotator::ZeroRotator;

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = NewPlayer;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    if (PlayerPreviewClass)
    {
        APlayerPreview* Preview = GetWorld()->SpawnActor<APlayerPreview>(PlayerPreviewClass, SpawnLocation, SpawnRotation, SpawnParams);
        if (Preview)
        {
            Preview->SetOwner(NewPlayer);

            if (ARelicRunnersPlayerController* RRPC = Cast<ARelicRunnersPlayerController>(NewPlayer))
            {
                RRPC->PlayerPreviewInstance = Preview;
                RRPC->TrySetupPreviewRenderTarget();
                UE_LOG(LogTemp, Warning, TEXT("[GameMode] Spawned preview actor for %s: %s"), *NewPlayer->GetName(), *Preview->GetName());
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[GameMode] Failed to spawn preview actor for %s"), *NewPlayer->GetName());
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[GameMode] PlayerPreviewClass not set!"));
    }
}
