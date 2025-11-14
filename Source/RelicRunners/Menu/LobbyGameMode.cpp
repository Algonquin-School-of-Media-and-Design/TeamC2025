// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "RelicRunners/PlayerPreview/LobbyPreview.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "RelicRunners/PlayerController/RelicRunnersPlayerController.h"
#include "RelicRunners/Menu/JoinUserWidget.h"
#include "GameFramework/HUD.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "RelicRunners/PlayerState/RelicRunnersPlayerState.h"
#include <OnlineSessionSettings.h>

ALobbyGameMode::ALobbyGameMode()
{
    // Default spawn positions (your pillar coords)
    LobbySpawnPositions = {
        FVector(-2235,110,330),
        FVector(-2235,230,330),
        FVector(-2319,340,340),
        FVector(-2320,0,340)
    };

    PlayerControllerClass = ARelicRunnersPlayerController::StaticClass();

    bUseSeamlessTravel = true;
    bReplicates = true;
}

void ALobbyGameMode::BeginPlay()
{
    Super::BeginPlay();
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if (!HasAuthority()) return; 

    if (!NewPlayer || !LobbyPreviewClass) return;

    PlayersInLobby.Add(NewPlayer);

    int32 Index = PlayersInLobby.Num() - 1;
    if (LobbySpawnPositions.IsValidIndex(Index))
    {
        FActorSpawnParameters Params;
        Params.Owner = NewPlayer;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        ALobbyPreview* SpawnedPreview = GetWorld()->SpawnActor<ALobbyPreview>(LobbyPreviewClass, LobbySpawnPositions[Index], FRotator::ZeroRotator, Params);

        if (SpawnedPreview)
        {
            if (ARelicRunnersPlayerState* RPS = NewPlayer->GetPlayerState<ARelicRunnersPlayerState>())
            {
                SpawnedPreview->SetupFromPlayerState(RPS);
                LobbyPreviews.Add(NewPlayer, SpawnedPreview);
            }
            else
            {
                SpawnedPreview->Destroy();
            }
        }
    }

    TWeakObjectPtr<ALobbyGameMode> WeakThis(this);
    TSharedRef<int32> RepeatCount = MakeShared<int32>(0);
    TSharedRef<FTimerHandle> TimerHandle_Update = MakeShared<FTimerHandle>();

    GetWorld()->GetTimerManager().SetTimer(*TimerHandle_Update, [WeakThis, TimerHandle_Update, RepeatCount]()
    {
            if (!WeakThis.IsValid()) return;

            WeakThis->UpdateSetup();

            (*RepeatCount)++;
            if (*RepeatCount >= 4)
            {
                if (UWorld* World = WeakThis->GetWorld())
                {
                    World->GetTimerManager().ClearTimer(*TimerHandle_Update);
                }
            }
    }, 0.5f, true);
}

void ALobbyGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);

    APlayerController* ExitingPC = Cast<APlayerController>(Exiting);
    if (!ExitingPC) return;

    // Remove from lobby array
    PlayersInLobby.Remove(ExitingPC);

    // Destroy their preview actor if exists
    ALobbyPreview* Preview = nullptr;
    if (LobbyPreviews.RemoveAndCopyValue(ExitingPC, Preview))
    {
        if (HasAuthority() && Preview)
        {
            Preview->Destroy();
        }
    }

    UpdateSetup();
}

void ALobbyGameMode::UpdateSetup()
{
    for (APlayerController* PC : PlayersInLobby)
    {
        if (ARelicRunnersPlayerController* RRPC = Cast<ARelicRunnersPlayerController>(PC))
        {
            RRPC->Client_UpdateLobbyUI();
            RRPC->Client_SetupLobby();
        }
    }
}
