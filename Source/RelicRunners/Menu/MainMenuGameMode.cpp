#include "MainMenuGameMode.h"
#include "MainMenuWidget.h" 
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "RelicRunners/Menu/JoinUserWidget.h"
#include "RelicRunners/Menu/ModeSelectionWidget.h"

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
    UWorld* World = GetWorld();
    if (World)
    {
        APlayerController* PC = World->GetFirstPlayerController();

        TArray<AActor*> FoundActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundActors);
        AActor* MainMenuCamera = nullptr;

        for (auto actor : FoundActors)
        {
            if (actor->ActorHasTag("MainMenuCamera"))
            {
                MainMenuCamera = actor;
            }
        }

        if (MainMenuCamera)
        {
            PC->SetViewTargetWithBlend(MainMenuCamera, 0.0f);
        }

        if (LobbyWidget)
        {
            LobbyWidget->RemoveFromParent();
        }

        if (!MainMenuWidget && MainMenuWidgetClass)
        {
            MainMenuWidget = CreateWidget<UMainMenuWidget>(PC, MainMenuWidgetClass);
            if (MainMenuWidget)
            {
                MainMenuWidget->AddToViewport();
                MainMenuWidget->SetVisibility(ESlateVisibility::Visible);
                MainMenuWidget->SetIsEnabled(true);
                PC->SetInputMode(FInputModeGameAndUI());
                PC->SetShowMouseCursor(true);
            }
        }
    }

    Super::BeginPlay();
}
