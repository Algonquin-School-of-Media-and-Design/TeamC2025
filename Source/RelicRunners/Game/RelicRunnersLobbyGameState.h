// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "RelicRunnersLobbyGameState.generated.h"

UCLASS()
class ARelicRunnersLobbyGameState : public AGameState
{
    GENERATED_BODY()

public:
    virtual void AddPlayerState(APlayerState* PlayerState) override;
    virtual void RemovePlayerState(APlayerState* PlayerState) override;
    void RefreshLobbyPreviews();

protected:
    UPROPERTY()
    TArray<class ALobbyPreview*> PreviewActors;
};


