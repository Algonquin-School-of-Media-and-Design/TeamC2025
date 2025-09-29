// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "AdvancedFriendsGameInstance.h"
#include "RelicRunnersGameInstance.generated.h"

UCLASS()
class RELICRUNNERS_API URelicRunnersGameInstance : public UAdvancedFriendsGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;
    virtual void Shutdown() override;

    void HostGame();
    void FindGames(class UJoinUserWidget* UserWidget);
    void JoinGame(int32 SessionIndex);
    void LeaveSession();

    void SetCharacterName(const FString& NewName);
    FString GetCharacterName();

private:
    /** Pointer to the session interface */
    IOnlineSessionPtr SessionInterface;

    /** Active search handle */
    TSharedPtr<FOnlineSessionSearch> SessionSearch;

    /** Widget that will display session search results */
    class UJoinUserWidget* TextRenderWidget;

    /** Cached player name */
    FString PlayerName;

    /** Delegate handlers */
    void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
    void OnFindSessionsComplete(bool bWasSuccessful);
    void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
    void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

    UPROPERTY(EditAnywhere)
    TSubclassOf<class UJoinUserWidget> LobbyWidgetClass = nullptr;

    UPROPERTY()
    class UJoinUserWidget* LobbyWidget = nullptr;

    UPROPERTY(EditAnywhere)
    TSubclassOf<class UMainMenuWidget> MainMenuWidgetClass = nullptr;

    UPROPERTY()
    class UMainMenuWidget* MainMenuWidget = nullptr;
};
