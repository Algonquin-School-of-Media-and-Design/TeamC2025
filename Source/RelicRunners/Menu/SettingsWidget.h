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
	void SetParentMenu(class UMainMenuWidget* InParentMenu);
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* BackButton = nullptr;
	UPROPERTY()
	class UMainMenuWidget* ParentMenu;

	UPROPERTY(meta = (BindWidget))

	class UTileView* KeybindingsTileView;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Keybindings")
	TArray<class UKeybindingsListData*> DefaultKeybindings;

	UPROPERTY(BlueprintReadWrite)
	bool InvertedXMouse = false;

	UPROPERTY(BlueprintReadWrite)
	bool InvertedYMouse = true;
	UFUNCTION(BlueprintCallable)
	void ToggleInvertedXMouse() {
		if (InvertedXMouse) { InvertedXMouse = false; }
		else { InvertedXMouse = true; }
	}
	UFUNCTION(BlueprintCallable)
	void ToggleInvertedYMouse() {
		if (InvertedYMouse) { InvertedYMouse = false; }
		else { InvertedYMouse = true; }
	}

	UFUNCTION(BlueprintCallable)
	void ApplyKeybindings();
protected:
	UFUNCTION()
	void BackButtonClicked();
	void InitializeDefaultKeybindings();

};
