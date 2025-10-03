// Fill out your copyright notice in the Description page of Project Settings.

#include "RelicRunnersLobbyGameState.h"
#include "GameFramework/PlayerState.h"
#include "RelicRunners/PlayerPreview/LobbyPreview.h"
#include "Engine/World.h"

void ARelicRunnersLobbyGameState::AddPlayerState(APlayerState* PlayerState)
{
    Super::AddPlayerState(PlayerState);
    RefreshLobbyPreviews();
}

void ARelicRunnersLobbyGameState::RemovePlayerState(APlayerState* PlayerState)
{
    Super::RemovePlayerState(PlayerState);
    RefreshLobbyPreviews();
}

void ARelicRunnersLobbyGameState::OnRep_PlayerArray()
{
    RefreshLobbyPreviews();
}

void ARelicRunnersLobbyGameState::RefreshLobbyPreviews()
{
    // Clean up existing preview actors
    for (auto* Preview : PreviewActors)
    {
        if (Preview) Preview->Destroy();
    }
    PreviewActors.Empty();

    // For each player in lobby, spawn a local preview actor
    int32 Index = 0;
    for (APlayerState* PS : PlayerArray)
    {
        if (!PS) continue;

        // Get spawn point for this slot (pillar locations)
        FVector SpawnLoc = FVector(300 * Index, 0, 100); // Replace with real pillar positions
        FRotator SpawnRot = FRotator::ZeroRotator;

        ALobbyPreview* Preview = GetWorld()->SpawnActor<ALobbyPreview>(ALobbyPreview::StaticClass(), SpawnLoc, SpawnRot);
        if (Preview)
        {
            // Setup meshes based on PlayerState (name, class, cosmetics, etc.)
            Preview->SetupFromPlayerState(PS);

            PreviewActors.Add(Preview);
        }

        Index++;
    }
}