// Fill out your copyright notice in the Description page of Project Settings.

#include "RelicRunnersLobbyGameState.h"
#include "GameFramework/PlayerState.h"
#include "RelicRunners/PlayerPreview/LobbyPreview.h"
#include "RelicRunners/PlayerState/RelicRunnersPlayerState.h"
#include "Engine/World.h"

void ARelicRunnersLobbyGameState::AddPlayerState(APlayerState* PlayerState)
{
    Super::AddPlayerState(PlayerState);
    RefreshLobbyPreviews();
}

void ARelicRunnersLobbyGameState::RemovePlayerState(APlayerState* PlayerState)
{
    Super::RemovePlayerState(PlayerState);

    for (int i = PreviewActors.Num() - 1; i >= 0; --i)
    {
        if (PreviewActors[i] && PreviewActors[i]->LinkedPlayerState == PlayerState)
        {
            PreviewActors[i]->Destroy();
            PreviewActors.RemoveAt(i);
        }
    }
}

void ARelicRunnersLobbyGameState::RefreshLobbyPreviews()
{
    if (!HasAuthority()) return;

    for (APlayerState* PS : PlayerArray)
    {
        if (!PS) continue;

        // Skip if already has a lobby preview
        bool AlreadyExists = false;
        for (auto* ExistingPreview : PreviewActors)
        {
            if (ExistingPreview && ExistingPreview->LinkedPlayerState == PS)
            {
                AlreadyExists = true;
                break;
            }
        }
        if (AlreadyExists) continue;

        // Spawn new lobby preview actor
        int Index = PreviewActors.Num();
        FVector SpawnLoc = FVector(300 * Index, 0, 100);
        FRotator SpawnRot = FRotator::ZeroRotator;

        ALobbyPreview* Preview = GetWorld()->SpawnActor<ALobbyPreview>(ALobbyPreview::StaticClass(), SpawnLoc, SpawnRot);

        if (Preview)
        {
            ARelicRunnersPlayerState* RPS = Cast<ARelicRunnersPlayerState>(PS);
            Preview->SetupFromPlayerState(RPS);
            PreviewActors.Add(Preview);
        }
    }
}