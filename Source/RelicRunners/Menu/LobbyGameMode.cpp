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

    // IMPORTANT: ensure local fogs show up at close range — set the console var to 0 (or small)
    if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.LocalFogVolume.GlobalStartDistance")))
    {
        CVar->Set(0); // default is 2000 (20m). Set to 0 to disable the "start distance" culling.
    }
}

void ALobbyGameMode::BeginPlay()
{
    Super::BeginPlay();
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if (!HasAuthority()) return; // Only the server should do spawn logic

    if (!NewPlayer || !LobbyPreviewClass) return;

    PlayersInLobby.Add(NewPlayer);

    int32 Index = PlayersInLobby.Num() - 1;
    if (LobbySpawnPositions.IsValidIndex(Index))
    {
        FActorSpawnParameters Params;
        Params.Owner = NewPlayer;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        ALobbyPreview* SpawnedPreview = GetWorld()->SpawnActor<ALobbyPreview>(
            LobbyPreviewClass,
            LobbySpawnPositions[Index],
            FRotator::ZeroRotator,
            Params
        );

        if (SpawnedPreview)
        {
            // Link preview to this player's PlayerState on the server
            if (ARelicRunnersPlayerState* RPS = NewPlayer->GetPlayerState<ARelicRunnersPlayerState>())
            {
                SpawnedPreview->SetupFromPlayerState(RPS);
                LobbyPreviews.Add(NewPlayer, SpawnedPreview);
            }
            else
            {
                // No player state (shouldn't happen), destroy preview
                SpawnedPreview->Destroy();
            }
        }
    }

    UpdateSetup();
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
