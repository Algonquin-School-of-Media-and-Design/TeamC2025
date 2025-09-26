#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ModeSelectionWidget.generated.h"

/**
 *
 */
UCLASS()
class RELICRUNNERS_API UModeSelectionWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void NativeConstruct() override;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* HostButton = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* SessionsButton = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* UpgradesButton = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* SettingsButton = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* TutorialButton = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* CreditsButton = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* PlayButton = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UEditableTextBox* NameTextBox = nullptr;

	void SetParentMenu(class UMainMenuWidget* InParentMenu);

protected:
	UFUNCTION()
	void HostButtonClicked();
	UFUNCTION()
	void SessionsButtonClicked();
	UFUNCTION()
	void SettingsButtonClicked();
	UFUNCTION()
	void CreditsButtonClicked();
	UFUNCTION()
	void TutorialButtonClicked();
	UFUNCTION()
	void UpgradesButtonClicked();
	UFUNCTION()
	void PlayButtonClicked();

	UPROPERTY()
	class UMainMenuWidget* ParentMenu;

	UFUNCTION()
	void OnPlayerNameTextChanged(const FText& Text);

	int32 MaxCharacterLimit;
};
