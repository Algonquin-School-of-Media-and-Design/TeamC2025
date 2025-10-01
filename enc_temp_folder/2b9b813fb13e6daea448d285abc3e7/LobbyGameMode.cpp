// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include <Kismet/GameplayStatics.h>
#include "RelicRunners/PlayerController/RelicRunnersPlayerController.h"

ALobbyGameMode::ALobbyGameMode()
{
    // No pawn should spawn in the menu
    DefaultPawnClass = nullptr;
    PlayerControllerClass = ARelicRunnersPlayerController::StaticClass();
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if (NewPlayer)
    {
        // Find camera in Lobby map
        AActor* LobbyCamera = nullptr;
        TArray<AActor*> FoundCameras;
        UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("LobbyCamera"), FoundCameras);

        if (FoundCameras.Num() > 0)
        {
            LobbyCamera = FoundCameras[0];
        }

        if (LobbyCamera)
        {
            NewPlayer->SetViewTargetWithBlend(LobbyCamera, 0.0f);
        }
    }
}
