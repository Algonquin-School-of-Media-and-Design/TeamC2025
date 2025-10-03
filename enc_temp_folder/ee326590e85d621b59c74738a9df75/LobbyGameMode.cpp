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
        FVector(-2320.f, -40.f, 340.f),
        FVector(-2235.f, 100.f, 330.f),
        FVector(-2235.f, 240.f, 330.f),
        FVector(-2320.f, 380.f, 340.f)
    };

    DefaultPawnClass = nullptr; // No pawn
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

    if (ARelicRunnersPlayerController* PC = Cast<ARelicRunnersPlayerController>(NewPlayer))
    {
        PC->ClientSetupLobby();
    }


    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (ARelicRunnersPlayerController* PC = Cast<ARelicRunnersPlayerController>(It->Get()))
        {
            PC->Client_UpdateLobbyUI();
        }
    }

    // Update all players' UI
    UpdateAllFindGamesButtons();
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

    // Update remaining players' UI
    UpdateAllFindGamesButtons();

    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (ARelicRunnersPlayerController* PC = Cast<ARelicRunnersPlayerController>(It->Get()))
        {
            PC->Client_UpdateLobbyUI();
        }
    }
}

void ALobbyGameMode::UpdateAllFindGamesButtons()
{
    for (APlayerController* PC : PlayersInLobby)
    {
        if (ARelicRunnersPlayerController* RRPC = Cast<ARelicRunnersPlayerController>(PC))
        {
            RRPC->Client_UpdateLobbyUI();
        }
    }
}
