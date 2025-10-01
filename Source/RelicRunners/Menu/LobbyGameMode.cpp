// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include <Kismet/GameplayStatics.h>
#include "RelicRunners/PlayerController/RelicRunnersPlayerController.h"
#include "RelicRunners/PlayerPreview/LobbyPreview.h"

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
        NewPlayer->bAutoManageActiveCameraTarget = false;

        TArray<AActor*> FoundCameras;
        UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("LobbyCamera"), FoundCameras);

        if (FoundCameras.Num() > 0)
        {
            AActor* LobbyCamera = FoundCameras[0];

            // Delay slightly in case PostLogin is too early
            FTimerHandle TimerHandle;
            GetWorldTimerManager().SetTimer(TimerHandle, [NewPlayer, LobbyCamera]()
                {
                    if (NewPlayer && LobbyCamera)
                    {
                        NewPlayer->SetViewTargetWithBlend(LobbyCamera, 0.0f);
                        UE_LOG(LogTemp, Log, TEXT("View set to LobbyCamera"));
                    }
                }, 0.1f, false);
        }
    }

    const FVector SpawnLocation = LobbySpawnPositions[0];
    const FRotator SpawnRotation = FRotator::ZeroRotator;

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = NewPlayer;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    if (LobbyPreviewClass)
    {
        ALobbyPreview* Preview = GetWorld()->SpawnActor<ALobbyPreview>(LobbyPreviewClass, SpawnLocation, SpawnRotation, SpawnParams);
        if (Preview)
        {
            Preview->SetOwner(NewPlayer);

            if (ARelicRunnersPlayerController* RRPC = Cast<ARelicRunnersPlayerController>(NewPlayer))
            {
                RRPC->LobbyPreviewInstance = Preview;
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

