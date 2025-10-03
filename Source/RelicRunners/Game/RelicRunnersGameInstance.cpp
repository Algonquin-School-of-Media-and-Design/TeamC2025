#include "RelicRunnersGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
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
#include "Interfaces/OnlineSessionInterface.h"
#include "Engine/Engine.h"

void URelicRunnersGameInstance::Init()
{
    Super::Init();

    IOnlineSubsystem* OSS = IOnlineSubsystem::Get();
    if (OSS)
    {
        SessionInterface = OSS->GetSessionInterface();
    }
    if (SessionInterface.IsValid())
    {
        UE_LOG(LogTemp, Log, TEXT("Session interface initialized"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to get session interface"));
    }
}

void URelicRunnersGameInstance::Shutdown()
{
    Super::Shutdown();
    LeaveSession();
}

/* Travel to main menu */
void URelicRunnersGameInstance::TravelToMainMenu()
{
    if (APlayerController* PC = GetFirstLocalPlayerController())
    {
        PC->ClientTravel(TEXT("/Game/ThirdPerson/Maps/MainMenu"), ETravelType::TRAVEL_Absolute);
    }
}

/* Back button: destroy any session then travel */
void URelicRunnersGameInstance::BackToMainMenu()
{
    if (TSharedPtr<IOnlineSession> Sess = SessionInterface.Pin())
    {
        if (Sess->GetNamedSession(NAME_GameSession))
        {
            // bind once, destroy, wait for callback
            Sess->OnDestroySessionCompleteDelegates.RemoveAll(this);
            Sess->OnDestroySessionCompleteDelegates.AddUObject(this, &URelicRunnersGameInstance::OnDestroySessionComplete);
            Sess->DestroySession(NAME_GameSession);
            return; // wait for delegate
        }
    }

    TravelToMainMenu();
}

/* Host flow: if existing session exists, destroy it and retry when destroyed */
void URelicRunnersGameInstance::HostGame()
{
    if (!SessionInterface.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot host: SessionInterface invalid"));
        return;
    }

    if (TSharedPtr<IOnlineSession> Sess = SessionInterface.Pin())
    {
        // If a named session exists locally, destroy it first and retry in callback
        if (Sess->GetNamedSession(NAME_GameSession))
        {
            UE_LOG(LogTemp, Warning, TEXT("Session already exists, destroying before creating a new one..."));

            // Queue the host attempt and bind delegate (remove old binds to avoid duplicates)
            PendingHostAfterLeave = 1;
            Sess->OnDestroySessionCompleteDelegates.RemoveAll(this);
            Sess->OnDestroySessionCompleteDelegates.AddUObject(this, &URelicRunnersGameInstance::OnDestroySessionComplete);
            Sess->DestroySession(NAME_GameSession);
            return; // wait for delegate
        }

        // no existing session create immediately
        CreateNewSession();
    }
}

void URelicRunnersGameInstance::CreateNewSession()
{
    if (!SessionInterface.IsValid()) return;
    if (TSharedPtr<IOnlineSession> Sess = SessionInterface.Pin())
    {
        FOnlineSessionSettings SessionSettings;
        SessionSettings.bIsLANMatch = false;
        SessionSettings.bShouldAdvertise = true;
        SessionSettings.bUsesPresence = true;
        SessionSettings.bAllowInvites = true;
        SessionSettings.bUseLobbiesIfAvailable = true;
        SessionSettings.bAllowJoinViaPresence = true;
        SessionSettings.bAllowJoinInProgress = true;
        SessionSettings.NumPublicConnections = 4;
        SessionSettings.Set(SETTING_MAPNAME, FString(TEXT("Lobby")), EOnlineDataAdvertisementType::ViaOnlineService);

        // bind and create
        Sess->OnCreateSessionCompleteDelegates.RemoveAll(this);
        Sess->OnCreateSessionCompleteDelegates.AddUObject(this, &URelicRunnersGameInstance::OnCreateSessionComplete);
        Sess->CreateSession(0, NAME_GameSession, SessionSettings);

        UE_LOG(LogTemp, Log, TEXT("CreateNewSession called"));
    }
}

void URelicRunnersGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    UE_LOG(LogTemp, Log, TEXT("OnCreateSessionComplete: %s success=%d"), *SessionName.ToString(), bWasSuccessful);
    if (bWasSuccessful)
    {
        UGameplayStatics::OpenLevel(this, TEXT("Lobby"), true, TEXT("listen"));
    }

    // unbind create delegate
    if (TSharedPtr<IOnlineSession> Sess = SessionInterface.Pin())
    {
        Sess->OnCreateSessionCompleteDelegates.RemoveAll(this);
    }
}

/* Find sessions */
void URelicRunnersGameInstance::FindGames(UJoinUserWidget* UserWidget)
{
    if (!SessionInterface.IsValid()) return;
    if (TSharedPtr<IOnlineSession> Sess = SessionInterface.Pin())
    {
        SessionSearch = MakeShareable(new FOnlineSessionSearch());
        SessionSearch->bIsLanQuery = false;
        SessionSearch->MaxSearchResults = 10;
        SessionSearch->QuerySettings.Set(FName("PRESENCESEARCH"), true, EOnlineComparisonOp::Equals);

        Sess->OnFindSessionsCompleteDelegates.RemoveAll(this);
        Sess->OnFindSessionsCompleteDelegates.AddUObject(this, &URelicRunnersGameInstance::OnFindSessionsComplete);
        Sess->FindSessions(0, SessionSearch.ToSharedRef());

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
                TextRenderWidget->OnFindSessionsComplete(MapName + " " + FString::FromInt(CurrentPlayers) + " out of " + FString::FromInt(MaxPlayers));
            }
        }
        else
        {
            TextRenderWidget->EnableRefresh();
        }
    }

    // unbind to be tidy
    if (TSharedPtr<IOnlineSession> Sess = SessionInterface.Pin())
    {
        Sess->OnFindSessionsCompleteDelegates.RemoveAll(this);
    }
}

/* Leave session: always destroy (client & host). Optionally queue host */
void URelicRunnersGameInstance::LeaveSession(bool bQueueHost /*= false*/)
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

            // clear local state so it won't reuse stale info
            PendingJoinIndex = -1;
            SessionSearch.Reset();
        }
    }
}

/* Called after a session is destroyed (common callback used for queued host/join or travel back) */
void URelicRunnersGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
    UE_LOG(LogTemp, Log, TEXT("OnDestroySessionComplete: %s success=%d"), *SessionName.ToString(), bWasSuccessful);

    // unbind delegate
    if (TSharedPtr<IOnlineSession> Sess = SessionInterface.Pin())
    {
        Sess->OnDestroySessionCompleteDelegates.RemoveAll(this);
    }

    // clear stale search/join state
    PendingJoinIndex = -1;
    SessionSearch.Reset();

    if (PendingHostAfterLeave > 0)
    {
        PendingHostAfterLeave = 0;
        // attempt host now that session was destroyed
        HostGame();
        return; // HostGame will create a new session
    }

    // If we had a pending join that specifically used a different callback, it will be handled by OnSessionDestroyedThenJoin
    // Otherwise just return to menu
    TravelToMainMenu();
}

/* Join flow: if we are currently in a session, destroy it and queue join */
void URelicRunnersGameInstance::JoinGame(int32 SessionIndex)
{
    if (!SessionInterface.IsValid() || !SessionSearch.IsValid() || SessionIndex < 0 || SessionIndex >= SessionSearch->SearchResults.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("JoinGame: invalid params"));
        return;
    }

    if (TSharedPtr<IOnlineSession> Sess = SessionInterface.Pin())
    {
        // if local "named" session exists, destroy it first and queue join
        if (Sess->GetNamedSession(NAME_GameSession))
        {
            UE_LOG(LogTemp, Log, TEXT("Already in a session, destroying before join..."));

            // queue and destroy
            PendingJoinIndex = SessionIndex;
            Sess->OnDestroySessionCompleteDelegates.RemoveAll(this);
            Sess->OnDestroySessionCompleteDelegates.AddUObject(this, &URelicRunnersGameInstance::OnSessionDestroyedThenJoin);
            Sess->DestroySession(NAME_GameSession);
            return;
        }

        // no existing local session -> join immediately
        Sess->OnJoinSessionCompleteDelegates.RemoveAll(this);
        Sess->OnJoinSessionCompleteDelegates.AddUObject(this, &URelicRunnersGameInstance::OnJoinSessionComplete);
        Sess->JoinSession(0, NAME_GameSession, SessionSearch->SearchResults[SessionIndex]);
    }
}

/* called when the destroyed session (before join) is completed */
void URelicRunnersGameInstance::OnSessionDestroyedThenJoin(FName SessionName, bool bWasSuccessful)
{
    UE_LOG(LogTemp, Log, TEXT("OnSessionDestroyedThenJoin: %s success=%d"), *SessionName.ToString(), bWasSuccessful);

    // unbind to be tidy
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

/* Join result resolve connect string and travel */
void URelicRunnersGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    FString ResultStr = FString::Printf(TEXT("OnJoinSessionComplete: SessionName=%s Result=%d"), *SessionName.ToString(), (int32)Result);
    UE_LOG(LogTemp, Log, TEXT("%s"), *ResultStr);
    if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Yellow, ResultStr);

    if (Result != EOnJoinSessionCompleteResult::Success)
    {
        FString FailMsg = FString::Printf(TEXT("JoinSession failed with result %d"), (int32)Result);
        UE_LOG(LogTemp, Warning, TEXT("%s"), *FailMsg);
        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Red, FailMsg);
        return;
    }

    if (TSharedPtr<IOnlineSession> Sess = SessionInterface.Pin())
    {
        FString ConnectInfo;
        // try game port first (some subsystems use NAME_GamePort)
        if (!Sess->GetResolvedConnectString(SessionName, ConnectInfo, NAME_GamePort) || ConnectInfo.IsEmpty())
        {
            if (!Sess->GetResolvedConnectString(SessionName, ConnectInfo) || ConnectInfo.IsEmpty())
            {
                FString NoConnect = FString::Printf(TEXT("Failed to resolve connect string for session %s"), *SessionName.ToString());
                UE_LOG(LogTemp, Error, TEXT("%s"), *NoConnect);
                if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Red, NoConnect);
                return;
            }
        }

        UE_LOG(LogTemp, Log, TEXT("Resolved connect string: %s"), *ConnectInfo);
        if (APlayerController* PC = GetFirstLocalPlayerController())
        {
            PC->ClientTravel(ConnectInfo, ETravelType::TRAVEL_Absolute);
        }
    }

    // unbind join delegate
    if (TSharedPtr<IOnlineSession> Sess = SessionInterface.Pin())
    {
        Sess->OnJoinSessionCompleteDelegates.RemoveAll(this);
    }
}

/* Utilities */
void URelicRunnersGameInstance::SetCharacterName(const FString& NewName) { PlayerName = NewName; }
FString URelicRunnersGameInstance::GetCharacterName() const { return PlayerName; }

bool URelicRunnersGameInstance::IsInSession() const
{
    if (TSharedPtr<IOnlineSession> Sess = SessionInterface.Pin())
    {
        return (Sess->GetNamedSession(NAME_GameSession) != nullptr);
    }
    return false;
}