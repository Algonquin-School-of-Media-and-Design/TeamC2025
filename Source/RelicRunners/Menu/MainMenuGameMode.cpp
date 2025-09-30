#include "MainMenuGameMode.h"
#include "MainMenuWidget.h" 
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "JoinUserWidget.h"
#include <RelicRunners/PlayerController/RelicRunnersPlayerController.h>

AMainMenuGameMode::AMainMenuGameMode()
{
    // No pawn should spawn in the menu
    DefaultPawnClass = nullptr;
    PlayerControllerClass = ARelicRunnersPlayerController::StaticClass();
}

void AMainMenuGameMode::BeginPlay()
{
    Super::BeginPlay();
}
