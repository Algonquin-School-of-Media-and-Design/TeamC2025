// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingsWidget.generated.h"

/**
 * 
 */
UCLASS()
class RELICRUNNERS_API USettingsWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	FReply HandleKeyBindPressed(FKey PressedKey);
	UFUNCTION(BlueprintCallable)
	void OnTileViewScrolled();
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	void SetParentMenu(class UMainMenuWidget* InParentMenu);
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* BackButton = nullptr;
	UPROPERTY()
	class UMainMenuWidget* ParentMenu;

	UPROPERTY(meta = (BindWidget))

	class UTileView* KeybindingsTileView;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Keybindings")
	TArray<class UKeybindingsListData*> DefaultKeybindings;

	UFUNCTION(BlueprintCallable)
	void ApplyKeybindings();

	UFUNCTION(BlueprintCallable)
	void ToggleInvertedXMouse();

	UFUNCTION(BlueprintCallable)
	void ToggleInvertedYMouse();

	void InitializeDefaultKeybindings();

	UPROPERTY()
	class UKeybindingsListWidget* WaitingForKeyEntry = nullptr;

	void StartRebinding(class UKeybindingsListWidget* Entry);
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UPROPERTY(meta = (BindWidget))
	class UButton* B_RestoreDefaults;

	UFUNCTION()
	void OnRestoreDefaultsClicked();

protected:
	UFUNCTION()
	void BackButtonClicked();

};
