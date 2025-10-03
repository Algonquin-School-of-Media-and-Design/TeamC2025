#include "MainMenuGameMode.h"
#include "MainMenuWidget.h" 
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "JoinUserWidget.h"
#include <RelicRunners/PlayerController/RelicRunnersPlayerController.h>
#include "RelicRunners/PlayerPreview/LobbyPreview.h"
#include "RelicRunners/Game/RelicRunnersLobbyGameState.h"

AMainMenuGameMode::AMainMenuGameMode()
{
    DefaultPawnClass = nullptr; // No pawn
    PlayerControllerClass = ARelicRunnersPlayerController::StaticClass();
}

void AMainMenuGameMode::BeginPlay()
{
    Super::BeginPlay();

    UWorld* World = GetWorld();
    if (!World) return;

    // Find MainMenuCamera
    TArray<AActor*> Cameras;
    UGameplayStatics::GetAllActorsWithTag(World, FName("MainMenuCamera"), Cameras);
    if (Cameras.Num() > 0)
    {
        if (APlayerController* PC = World->GetFirstPlayerController())
        {
            PC->SetViewTargetWithBlend(Cameras[0], 0.f);
            PC->bShowMouseCursor = true;
            PC->SetInputMode(FInputModeUIOnly());
        }
    }

    // Show Main Menu UI
    if (UMainMenuWidget* Menu = CreateWidget<UMainMenuWidget>(World, LoadClass<UMainMenuWidget>(nullptr, TEXT("/Game/Blueprints/Menu/BP_MainMenuWidget.BP_MainMenuWidget_C"))))
    {
        Menu->AddToViewport();
    }
}





