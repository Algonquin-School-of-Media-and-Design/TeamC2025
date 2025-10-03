// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "RelicRunnersLobbyGameState.generated.h"

UCLASS()
class ARelicRunnersLobbyGameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    // Called whenever the player array changes
    virtual void AddPlayerState(APlayerState* PlayerState) override;
    virtual void RemovePlayerState(APlayerState* PlayerState) override;

    // Called on clients when replicated player array updates
    UFUNCTION()
    void OnRep_PlayerArray();

    // Helper to update the preview actors
    void RefreshLobbyPreviews();

protected:
    // Local-only preview actors (not replicated)
    UPROPERTY()
    TArray<class ALobbyPreview*> PreviewActors;
};


