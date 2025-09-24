// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "JoinUserWidget.generated.h"

/**
 *
 */
class FOnlineSessionSearchResult;

UCLASS()
class RELICRUNNERS_API UJoinUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* BackButton = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* FindGames = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* JoinButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SearchProgress;

protected:
	UFUNCTION()
	void BackButtonClicked();
	UFUNCTION()
	void FindGamesButtonClicked();

	UFUNCTION()
	void JoinGameButtonClicked();

	void SearchForLanGames();

	UFUNCTION()
	void OnItemClicked(UObject* ClickedItem);

	UPROPERTY(meta = (BindWidget))

	class UListView* SessionListView;

	UObject* LastSelectedItem = nullptr;
	void ShowJoinButton(bool IsEnabled);

	UPROPERTY()
	class UMainMenuWidget* ParentMenu;
public:
	void OnFindSessionsComplete(FString Str);

	void EnableRefresh();

	void SetParentMenu(class UMainMenuWidget* InParentMenu);

	void RefreshMenu();
};
