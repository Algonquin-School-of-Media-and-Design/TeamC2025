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

    UFUNCTION(BlueprintCallable)
    void BackToMainMenu();

    UFUNCTION(BlueprintCallable)
    void StartSessionGame();
    UFUNCTION(BlueprintCallable)
    void OnStartSessionComplete(FName SessionName, bool bWasSuccessful, FString TargetMap);

    UFUNCTION(BlueprintCallable)
    void CreateNewSession();
    UFUNCTION(BlueprintCallable)
    void HostGame();
    void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

    void FindGames(class UJoinUserWidget* UserWidget);
    void OnFindSessionsComplete(bool bWasSuccessful);

    UFUNCTION(BlueprintCallable)
    void JoinGame(int32 SessionIndex);

    void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
    void OnSessionDestroyedThenJoin(FName SessionName, bool bWasSuccessful);

    UFUNCTION(BlueprintCallable)
    void LeaveSession(bool bQueueHost = false);
    void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

    bool IsHost() const;
    bool IsInSession() const;

    void SetCharacterName(const FString& NewName) { PlayerName = NewName; }
    FString GetCharacterName() const { return PlayerName; }

    TSharedPtr<class FOnlineSessionSearch> SessionSearch;

private:
    TWeakPtr<class IOnlineSession, ESPMode::ThreadSafe> SessionInterface;
    TWeakObjectPtr<class UJoinUserWidget> TextRenderWidget;

    int32 PendingHostAfterLeave = 0;
    int32 PendingJoinIndex = -1;

    FString PlayerName;
};