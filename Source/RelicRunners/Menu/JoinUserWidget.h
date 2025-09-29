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
	void HandleEntryGenerated(UUserWidget& EntryWidget);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* BackButton = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* FindGames = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* JoinButton = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UBorder* JoinBorder = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UBorder* SessionsBorder = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UCircularThrobber* RefreshProgress;

protected:
	UFUNCTION()
	void BackButtonClicked();
	UFUNCTION()
	void FindGamesButtonClicked();

	UFUNCTION()
	void JoinGameButtonClicked();

	UFUNCTION()
	void HandleSessionClicked(class USessionListItemData* ClickedSession);

	void SearchForLanGames();

	UPROPERTY(meta = (BindWidget))

	class UTileView* SessionTileView;

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
