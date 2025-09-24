#include "MainMenuGameMode.h"
#include "MainMenuWidget.h" 
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

AMainMenuGameMode::AMainMenuGameMode()
{
    // No pawn should spawn in the menu
    DefaultPawnClass = nullptr;
    static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_RelicRunnersPlayerController"));

    if (PlayerControllerBPClass.Class)
    {
        PlayerControllerClass = PlayerControllerBPClass.Class;
        UE_LOG(LogTemp, Warning, TEXT("[GameMode] Assigned PlayerControllerClass from constructor."));
    }

    static ConstructorHelpers::FClassFinder<UUserWidget> MainMenuWidgetBPClass(
        TEXT("/Game/Blueprints/Menu/BP_MainMenuWidget")
    );

    if (MainMenuWidgetBPClass.Succeeded())
    {
        MainMenuWidgetClass = MainMenuWidgetBPClass.Class;
        UE_LOG(LogTemp, Warning, TEXT("[MainMenuGameMode] Assigned MainMenuWidgetClass in constructor."));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[MainMenuGameMode] Failed to find MainMenu widget at path!"));
    }
}

void AMainMenuGameMode::BeginPlay()
{
    Super::BeginPlay();
}
