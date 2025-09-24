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
	class UButton* JoinButton = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UEditableTextBox* NameTextBox = nullptr;

	void SetParentMenu(class UMainMenuWidget* InParentMenu);

protected:
	UFUNCTION()
	void HostButtonClicked();
	UFUNCTION()
	void JoinButtonClicked();

	UPROPERTY()
	class UMainMenuWidget* ParentMenu;

	UFUNCTION()
	void OnPlayerNameTextChanged(const FText& Text);

	int32 MaxCharacterLimit;
};
