#include "ModeSelectionWidget.h"
#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "RelicRunners/Game/RelicRunnersGameInstance.h"
#include "Components/EditableTextBox.h"
#include <RelicRunners/PlayerState/RelicRunnersPlayerState.h>


void UModeSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &UModeSelectionWidget::HostButtonClicked);
	}

	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &UModeSelectionWidget::JoinButtonClicked);
	}

	if (NameTextBox)
	{
		NameTextBox->OnTextChanged.AddDynamic(this, &UModeSelectionWidget::OnPlayerNameTextChanged);
	}

	MaxCharacterLimit = 12;
}

void UModeSelectionWidget::SetParentMenu(UMainMenuWidget* InParentMenu)
{
	ParentMenu = InParentMenu;
}

void UModeSelectionWidget::HostButtonClicked()
{
	URelicRunnersGameInstance* GameInstance = Cast<URelicRunnersGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		if (HostButton)
		{
			HostButton->SetIsEnabled(false);
			static int32 PlayerIndex = 1;
			FString EnteredName = TEXT("Player") + FString::FromInt(PlayerIndex++);
			if (NameTextBox)
			{
				EnteredName = NameTextBox->GetText().ToString();
			}

			// Set GameInstance name
			GameInstance->SetCharacterName(EnteredName);

			GameInstance->HostGame();
		}
	}
}


void UModeSelectionWidget::JoinButtonClicked()
{
	if (ParentMenu)
	{
		URelicRunnersGameInstance* GameInstance = Cast<URelicRunnersGameInstance>(GetGameInstance());

		if (GameInstance && NameTextBox)
		{
			GameInstance->SetCharacterName(NameTextBox->GetText().ToString());
		}
		ParentMenu->ShowJoinGameWidget();
	}
}

void UModeSelectionWidget::OnPlayerNameTextChanged(const FText& Text)
{
	FString CurrentText = Text.ToString();

	if (CurrentText.Len() > MaxCharacterLimit)
	{
		// Truncate the text
		FString TruncatedText = CurrentText.Left(MaxCharacterLimit);
		NameTextBox->SetText(FText::FromString(TruncatedText));
	}
}

