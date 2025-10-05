// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "RelicRunners/PlayerPreview/LobbyPreview.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "RelicRunners/PlayerController/RelicRunnersPlayerController.h"
#include "RelicRunners/Menu/JoinUserWidget.h"
#include "GameFramework/HUD.h"

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
}

void ALobbyGameMode::BeginPlay()
{
    Super::BeginPlay();
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if (!NewPlayer || !LobbyPreviewClass) return;

    // Add to our lobby array
    PlayersInLobby.Add(NewPlayer);

    // Spawn lobby preview for this player
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
            LobbyPreviews.Add(NewPlayer, SpawnedPreview);
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
