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
    DefaultPawnClass = nullptr;
    PlayerControllerClass = ARelicRunnersPlayerController::StaticClass();

    bUseSeamlessTravel = true;
    bReplicates = true;
}

void AMainMenuGameMode::BeginPlay()
{
    Super::BeginPlay();
    // IMPORTANT: ensure local fogs show up at close range — set the console var to 0 (or small)
    //if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.LocalFogVolume.GlobalStartDistance")))
    //{
    //    CVar->Set(0); // default is 2000 (20m). Set to 0 to disable the "start distance" culling.
    //}
}






