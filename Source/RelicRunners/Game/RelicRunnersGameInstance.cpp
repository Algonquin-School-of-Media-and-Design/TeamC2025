#include "RelicRunnersGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include <Online/OnlineSessionNames.h>
#include "Kismet/GameplayStatics.h"
#include "OnlineSessionSettings.h"
#include "RelicRunners/Menu/JoinUserWidget.h"
#include "RelicRunners/Menu/MainMenuWidget.h"

void URelicRunnersGameInstance::Init()
{
    Super::Init();

    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (Subsystem)
    {
        SessionInterface = Subsystem->GetSessionInterface();
        if (SessionInterface.IsValid())
        {
            UE_LOG(LogTemp, Log, TEXT("[GameInstance] Session interface initialized successfully."));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("[GameInstance] Failed to get session interface!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[GameInstance] No online subsystem found!"));
    }
}

void URelicRunnersGameInstance::HostGame()
{
    if (!SessionInterface.IsValid()) return;

    if (SessionInterface->GetNamedSession(NAME_GameSession))
    {
        UE_LOG(LogTemp, Warning, TEXT("Session already exists, destroying before creating a new one..."));
        SessionInterface->DestroySession(NAME_GameSession);
    }

    FOnlineSessionSettings SessionSettings;
    SessionSettings.bIsLANMatch = false;
    SessionSettings.bShouldAdvertise = true;
    SessionSettings.bUsesPresence = true;
    SessionSettings.bAllowInvites = true;
    SessionSettings.bUseLobbiesIfAvailable = true;
    SessionSettings.bAllowJoinViaPresence = true;
    SessionSettings.bAllowJoinInProgress = true;
    SessionSettings.NumPublicConnections = 4;
    SessionSettings.Set(FName("MAPNAME"), FString("Session"), EOnlineDataAdvertisementType::ViaOnlineService);

    SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &URelicRunnersGameInstance::OnCreateSessionComplete);
    SessionInterface->CreateSession(0, NAME_GameSession, SessionSettings);
}

void URelicRunnersGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        UE_LOG(LogTemp, Log, TEXT("Session %s created successfully!"), *SessionName.ToString());

        UWorld* World = GetWorld();
        if (World)
        {
            APlayerController* PC = World->GetFirstPlayerController();
            TArray<AActor*> FoundActors;
            UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundActors);

            AActor* LobbyCamera = nullptr;
            for (auto actor : FoundActors)
            {
                if (actor->ActorHasTag("LobbyCamera"))
                {
                    LobbyCamera = actor;
                }
            }

            if (LobbyCamera)
            {
                PC->SetViewTargetWithBlend(LobbyCamera, 0.0f);
            }

        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create session!"));
    }
}

void URelicRunnersGameInstance::FindGames(UJoinUserWidget* UserWidget)
{
    if (!SessionInterface.IsValid()) return;

    SessionSearch = MakeShareable(new FOnlineSessionSearch());
    SessionSearch->bIsLanQuery = false;
    SessionSearch->MaxSearchResults = 10;
    SessionSearch->QuerySettings.Set(FName("PRESENCESEARCH"), true, EOnlineComparisonOp::Equals);

    SessionInterface->OnFindSessionsCompleteDelegates.RemoveAll(this);
    SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &URelicRunnersGameInstance::OnFindSessionsComplete);
    SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());

    TextRenderWidget = UserWidget;
}

void URelicRunnersGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
    if (bWasSuccessful && SessionSearch->SearchResults.Num() > 0)
    {
        UE_LOG(LogTemp, Log, TEXT("Found %d session(s)."), SessionSearch->SearchResults.Num());

        if (TextRenderWidget)
        {
            for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
            {
                FString MapName = TEXT("Unknown Map");
                if (SearchResult.Session.SessionSettings.Settings.Contains(FName("MAPNAME")))
                {
                    MapName = SearchResult.Session.SessionSettings.Settings.Find(FName("MAPNAME"))->Data.ToString();
                }

                int32 MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
                int32 CurrentPlayers = MaxPlayers - SearchResult.Session.NumOpenPublicConnections;

                TextRenderWidget->OnFindSessionsComplete(
                    FString::Printf(TEXT("%s %d | %d"), *MapName, CurrentPlayers, MaxPlayers)
                );
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No sessions found."));
        if (TextRenderWidget)
        {
            TextRenderWidget->EnableRefresh();
        }
    }
}

void URelicRunnersGameInstance::LeaveSession()
{
    if (SessionInterface.IsValid() && SessionInterface->GetNamedSession(NAME_GameSession))
    {
        SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &URelicRunnersGameInstance::OnDestroySessionComplete);
        SessionInterface->DestroySession(NAME_GameSession);
    }
}

void URelicRunnersGameInstance::SetCharacterName(const FString& NewName)
{
    PlayerName = NewName;
}

FString URelicRunnersGameInstance::GetCharacterName()
{
    return PlayerName;
}

void URelicRunnersGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        UE_LOG(LogTemp, Log, TEXT("Session destroyed successfully. Returning to MainMenu."));

        if (APlayerController* PlayerController = GetFirstLocalPlayerController())
        {
            PlayerController->ClientTravel("/Game/ThirdPerson/Maps/MainMenu", ETravelType::TRAVEL_Absolute);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to destroy session."));
    }
}

void URelicRunnersGameInstance::JoinGame(int32 SessionIndex)
{
    if (!SessionSearch.IsValid() || SessionSearch->SearchResults.Num() <= SessionIndex) return;
    if (!SessionInterface.IsValid()) return;

    SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &URelicRunnersGameInstance::OnJoinSessionComplete);
    SessionInterface->JoinSession(0, NAME_GameSession, SessionSearch->SearchResults[SessionIndex]);
}

void URelicRunnersGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    if (!SessionInterface.IsValid()) return;

    FString ConnectString;
    if (SessionInterface->GetResolvedConnectString(SessionName, ConnectString))
    {
        if (APlayerController* PC = GetFirstLocalPlayerController())
        {
            UE_LOG(LogTemp, Log, TEXT("Joining session. Traveling to: %s"), *ConnectString);
            PC->ClientTravel(ConnectString, ETravelType::TRAVEL_Absolute);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Could not resolve connect string for session %s"), *SessionName.ToString());
    }
}

void URelicRunnersGameInstance::Shutdown()
{
    // Clean up session interface, clear delegates, etc.

    if (SessionInterface.IsValid())
    {
        SessionInterface->ClearOnCreateSessionCompleteDelegates(this);
        SessionInterface->ClearOnFindSessionsCompleteDelegates(this);
        SessionInterface->ClearOnDestroySessionCompleteDelegates(this);
    }

    Super::Shutdown();
}
