// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "AdvancedFriendsGameInstance.h"
#include "RelicRunnersGameInstance.generated.h"

UCLASS()
class RELICRUNNERS_API URelicRunnersGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;
    virtual void Shutdown() override;

    /** UI / travel */
    UFUNCTION(BlueprintCallable)
    void BackToMainMenu();
    void TravelToMainMenu();

    /** Host */
    UFUNCTION(BlueprintCallable)
    void HostGame();
    void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

    /** Find */
    void FindGames(class UJoinUserWidget* UserWidget);
    void OnFindSessionsComplete(bool bWasSuccessful);

    /** Join */
    UFUNCTION(BlueprintCallable)
    void JoinGame(int32 SessionIndex);
    void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
    void OnSessionDestroyedThenJoin(FName SessionName, bool bWasSuccessful);

    /** Leave / Destroy */
    UFUNCTION(BlueprintCallable)
    void LeaveSession(bool bQueueHost = false);
    void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

    /** helpers */
    void SetCharacterName(const FString& NewName);
    FString GetCharacterName() const;
    bool IsInSession() const;

private:
    /** backing data */
    TWeakPtr<class IOnlineSession, ESPMode::ThreadSafe> SessionInterface;
    TSharedPtr<class FOnlineSessionSearch> SessionSearch;

    /** UI kit (weak) */
    TWeakObjectPtr<class UJoinUserWidget> TextRenderWidget;

    /** queued actions */
    int32 PendingHostAfterLeave = 0;
    int32 PendingJoinIndex = -1;

    /** player name */
    FString PlayerName;

    /** internal helper */
    void CreateNewSession();
};