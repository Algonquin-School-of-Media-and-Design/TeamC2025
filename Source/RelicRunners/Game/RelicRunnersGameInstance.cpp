#include "RelicRunnersGameInstance.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include <Online/OnlineSessionNames.h>
#include "Kismet/GameplayStatics.h"
#include "OnlineSessionSettings.h"
#include "RelicRunners/Menu/JoinUserWidget.h"
#include "RelicRunners/Menu/MainMenuWidget.h"
#include <RelicRunners/Menu/MainMenuGameMode.h>
#include <RelicRunners/PlayerController/RelicRunnersPlayerController.h>
#include "RelicRunners/PlayerPreview/LobbyPreview.h"
#include "GameFramework/GameStateBase.h"
#include "Engine/Engine.h"
#include <RelicRunners/Menu/LobbyGameMode.h>

void URelicRunnersGameInstance::Init()
{
    Super::Init();

    //Setup online systems
    IOnlineSessionPtr OnlineSubsystem = Online::GetSessionInterface(GetWorld());
    if (OnlineSubsystem)
    {
        SessionInterface = OnlineSubsystem;
    }
}

void URelicRunnersGameInstance::Shutdown()
{
    Super::Shutdown();

    //Leave any remaining sessions
    LeaveSession();
}

void URelicRunnersGameInstance::BackToMainMenu()
{
    //Destroy session
    if (TSharedPtr<IOnlineSession> Sess = SessionInterface.Pin())
    {
        if (Sess->GetNamedSession(NAME_GameSession))
        {
            Sess->OnDestroySessionCompleteDelegates.RemoveAll(this);
            Sess->OnDestroySessionCompleteDelegates.AddUObject(this, &URelicRunnersGameInstance::OnDestroySessionComplete);
            Sess->DestroySession(NAME_GameSession);
            return;
        }
    }

    //Travel to main menu
    if (APlayerController* PC = GetFirstLocalPlayerController())
    {
        PC->ClientTravel(TEXT("/Game/ThirdPerson/Maps/MainMenu"), ETravelType::TRAVEL_Absolute);
    }
}

void URelicRunnersGameInstance::HostGame()
{
    if (!SessionInterface.IsValid()) return;

    if (TSharedPtr<IOnlineSession> Sess = SessionInterface.Pin())
    {
        //If a named session exists locally, destroy it first and retry in callback
        if (Sess->GetNamedSession(NAME_GameSession))
        {
            PendingHostAfterLeave = 1;
            Sess->OnDestroySessionCompleteDelegates.RemoveAll(this);
            Sess->OnDestroySessionCompleteDelegates.AddUObject(this, &URelicRunnersGameInstance::OnDestroySessionComplete);
            Sess->DestroySession(NAME_GameSession);
            return;
        }

        //If no existing session, create immediately
        CreateNewSession();
    }
}

void URelicRunnersGameInstance::CreateNewSession()
{
    if (!SessionInterface.IsValid()) return;

    if (TSharedPtr<IOnlineSession> Sess = SessionInterface.Pin())
    {
        //Setup parameters
        int LobbySize = 4;
        FOnlineSessionSettings SessionSettings;
        SessionSettings.NumPublicConnections = LobbySize;
        SessionSettings.bIsLANMatch = false;
        SessionSettings.bShouldAdvertise = true;
        SessionSettings.bUsesPresence = true;
        SessionSettings.bAllowInvites = true;
        SessionSettings.bUseLobbiesIfAvailable = true;
        SessionSettings.bAllowJoinViaPresence = true;
        SessionSettings.bAllowJoinInProgress = true;
        SessionSettings.Set(SETTING_MAPNAME, FString(TEXT("Lobby")), EOnlineDataAdvertisementType::ViaOnlineService);

        Sess->OnCreateSessionCompleteDelegates.RemoveAll(this);
        Sess->OnCreateSessionCompleteDelegates.AddUObject(this, &URelicRunnersGameInstance::OnCreateSessionComplete);
        Sess->CreateSession(0, NAME_GameSession, SessionSettings);
    }
}

void URelicRunnersGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    UE_LOG(LogTemp, Log, TEXT("OnCreateSessionComplete: %s success=%d"), *SessionName.ToString(), bWasSuccessful);
    if (bWasSuccessful)
    {
        UGameplayStatics::OpenLevel(this, TEXT("Lobby"), true, TEXT("listen"));
    }

    //Unbind delegate 
    if (TSharedPtr<IOnlineSession> Sess = SessionInterface.Pin())
    {
        Sess->OnCreateSessionCompleteDelegates.RemoveAll(this);
    }
}

void URelicRunnersGameInstance::FindGames(UJoinUserWidget* UserWidget)
{
    if (TSharedPtr<IOnlineSession> Session = SessionInterface.Pin())
    {
        //Search parameters
        int NumResults = 10;
        SessionSearch = MakeShareable(new FOnlineSessionSearch());
        SessionSearch->MaxSearchResults = NumResults;
        SessionSearch->bIsLanQuery = false;
        SessionSearch->QuerySettings.Set(FName("PRESENCESEARCH"), true, EOnlineComparisonOp::Equals);

        Session->OnFindSessionsCompleteDelegates.RemoveAll(this);
        Session->OnFindSessionsCompleteDelegates.AddUObject(this, &URelicRunnersGameInstance::OnFindSessionsComplete);
        Session->FindSessions(0, SessionSearch.ToSharedRef());

        TextRenderWidget = UserWidget;
    }
}

void URelicRunnersGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
    if (TextRenderWidget.IsValid())
    {
        if (bWasSuccessful && SessionSearch.IsValid() && SessionSearch->SearchResults.Num() > 0)
        {
            for (const FOnlineSessionSearchResult& R : SessionSearch->SearchResults)
            {
                FString MapName = R.Session.SessionSettings.Settings.Contains("MAPNAME")
                    ? *R.Session.SessionSettings.Settings.Find("MAPNAME")->Data.ToString()
                    : TEXT("Unknown Map");

                int32 MaxPlayers = R.Session.SessionSettings.NumPublicConnections;
                int32 CurrentPlayers = MaxPlayers - R.Session.NumOpenPublicConnections;

                TextRenderWidget->OnFindSessionsComplete(MapName + " " + FString::FromInt(CurrentPlayers) + " | " + FString::FromInt(MaxPlayers));
            }
        }
        else
        {
            TextRenderWidget->EnableRefresh();
        }
    }

    //Unbind delegate
    if (TSharedPtr<IOnlineSession> Sess = SessionInterface.Pin())
    {
        Sess->OnFindSessionsCompleteDelegates.RemoveAll(this);
    }
}

void URelicRunnersGameInstance::LeaveSession(bool bQueueHost)
{
    if (!SessionInterface.IsValid()) return;

    if (TSharedPtr<IOnlineSession> Sess = SessionInterface.Pin())
    {
        if (Sess->GetNamedSession(NAME_GameSession))
        {
            if (bQueueHost)
            {
                PendingHostAfterLeave = 1;
            }

            Sess->OnDestroySessionCompleteDelegates.RemoveAll(this);
            Sess->OnDestroySessionCompleteDelegates.AddUObject(this, &URelicRunnersGameInstance::OnDestroySessionComplete);
            Sess->DestroySession(NAME_GameSession);

            // Clear local state so it won't reuse stale info
            PendingJoinIndex = -1;
            SessionSearch.Reset();
        }
    }
}

void URelicRunnersGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
    UE_LOG(LogTemp, Log, TEXT("OnDestroySessionComplete: %s success=%d"), *SessionName.ToString(), bWasSuccessful);

    //Unbind delegate
    if (TSharedPtr<IOnlineSession> Sess = SessionInterface.Pin())
    {
        Sess->OnDestroySessionCompleteDelegates.RemoveAll(this);
    }

    //Clear stale search/join state
    PendingJoinIndex = -1;
    SessionSearch.Reset();

    if (PendingHostAfterLeave > 0)
    {
        PendingHostAfterLeave = 0;

        //Attempt host now that session was destroyed
        HostGame();
        return;
    }

    //Travel to main menu
    if (APlayerController* PC = GetFirstLocalPlayerController())
    {
        PC->ClientTravel(TEXT("/Game/ThirdPerson/Maps/MainMenu"), ETravelType::TRAVEL_Absolute);
    }
}

void URelicRunnersGameInstance::JoinGame(int32 SessionIndex)
{
    if (!SessionInterface.IsValid() || !SessionSearch.IsValid() || SessionIndex < 0 || SessionIndex >= SessionSearch->SearchResults.Num()) return;

    if (TSharedPtr<IOnlineSession> Sess = SessionInterface.Pin())
    {
        //If local session exists, destroy it first and queue join
        if (Sess->GetNamedSession(NAME_GameSession))
        {
            PendingJoinIndex = SessionIndex;
            Sess->OnDestroySessionCompleteDelegates.RemoveAll(this);
            Sess->OnDestroySessionCompleteDelegates.AddUObject(this, &URelicRunnersGameInstance::OnSessionDestroyedThenJoin);
            Sess->DestroySession(NAME_GameSession);
            return;
        }

        //No existing local session, join immediately
        Sess->OnJoinSessionCompleteDelegates.RemoveAll(this);
        Sess->OnJoinSessionCompleteDelegates.AddUObject(this, &URelicRunnersGameInstance::OnJoinSessionComplete);
        Sess->JoinSession(0, NAME_GameSession, SessionSearch->SearchResults[SessionIndex]);
        ClientTravelToSession(0, NAME_GameSession);
    }
}

void URelicRunnersGameInstance::OnSessionDestroyedThenJoin(FName SessionName, bool bWasSuccessful)
{
    UE_LOG(LogTemp, Log, TEXT("OnSessionDestroyedThenJoin: %s success=%d"), *SessionName.ToString(), bWasSuccessful);

    //Unbind to be tidy
    if (TSharedPtr<IOnlineSession> Sess = SessionInterface.Pin())
    {
        Sess->OnDestroySessionCompleteDelegates.RemoveAll(this);
    }

    if (!bWasSuccessful)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to destroy session before join"));
        PendingJoinIndex = -1;
        return;
    }

    if (PendingJoinIndex < 0 || !SessionSearch.IsValid() || PendingJoinIndex >= SessionSearch->SearchResults.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("PendingJoinIndex invalid or session search gone"));
        PendingJoinIndex = -1;
        return;
    }

    if (TSharedPtr<IOnlineSession> Sess = SessionInterface.Pin())
    {
        Sess->OnJoinSessionCompleteDelegates.RemoveAll(this);
        Sess->OnJoinSessionCompleteDelegates.AddUObject(this, &URelicRunnersGameInstance::OnJoinSessionComplete);
        Sess->JoinSession(0, NAME_GameSession, SessionSearch->SearchResults[PendingJoinIndex]);
    }

    PendingJoinIndex = -1;
}

void URelicRunnersGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    if (TSharedPtr<IOnlineSession> Sess = SessionInterface.Pin())
    {
        //Try game port first
        FString ConnectInfo;
        if (!Sess->GetResolvedConnectString(SessionName, ConnectInfo, NAME_GamePort) || ConnectInfo.IsEmpty())
        {
            if (!Sess->GetResolvedConnectString(SessionName, ConnectInfo) || ConnectInfo.IsEmpty())
            {
                FString NoConnect = FString::Printf(TEXT("Failed to resolve connect string for session %s"), *SessionName.ToString());
                UE_LOG(LogTemp, Error, TEXT("%s"), *NoConnect);
                return;
            }
        }

        if (APlayerController* PC = GetFirstLocalPlayerController())
        {
            PC->ClientTravel(ConnectInfo, ETravelType::TRAVEL_Absolute);
        }
    }

    //Unbind join delegate
    if (TSharedPtr<IOnlineSession> Sess = SessionInterface.Pin())
    {
        Sess->OnJoinSessionCompleteDelegates.RemoveAll(this);
    }
}

void URelicRunnersGameInstance::StartSessionGame()
{
    UWorld* World = GetWorld();
    if (!World) return;

    FString TravelURL = TEXT("/Game/ThirdPerson/Maps/ThirdPersonMap?listen");
    UE_LOG(LogTemp, Warning, TEXT("[LobbyGameMode] Host starting travel to %s"), *TravelURL);

    TSharedPtr<IOnlineSession> Sess = SessionInterface.Pin();

    FOnlineSessionSettings* CurrentSettings = Sess->GetSessionSettings(NAME_GameSession);
    if (CurrentSettings)
    {
        CurrentSettings->Set(SETTING_MAPNAME, FString(TEXT("Game")), EOnlineDataAdvertisementType::ViaOnlineService);
        Sess->UpdateSession(NAME_GameSession, *CurrentSettings, true);
    }

    // Tell all clients to prepare for travel
    for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
    {
        ARelicRunnersPlayerController* PC = Cast<ARelicRunnersPlayerController>(*Iterator);
        if (PC && !PC->IsLocalController()) // skip host
        {
            PC->ClientTravelToGame();
        }
    }

    // Host starts seamless travel
    World->SeamlessTravel(TravelURL);
}