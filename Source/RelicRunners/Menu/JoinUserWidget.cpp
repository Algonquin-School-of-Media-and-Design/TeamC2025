// Fill out your copyright notice in the Description page of Project Settings.


#include "JoinUserWidget.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "RelicRunners/Game/RelicRunnersGameInstance.h"
#include "Components/Button.h"
#include "Components/ListView.h"
#include "SessionListItemWidget.h"
#include "SessionListItemData.h"
#include "Components/TextBlock.h"
#include "MainMenuWidget.h"


void UJoinUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &UJoinUserWidget::BackButtonClicked);
	}

	if (FindGames)
	{
		FindGames->OnClicked.AddDynamic(this, &UJoinUserWidget::FindGamesButtonClicked);
	}

	if (SessionListView)
	{
		SessionListView->OnItemClicked().AddUObject(this, &UJoinUserWidget::OnItemClicked);
	}

	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &UJoinUserWidget::JoinGameButtonClicked);
	}

}

void UJoinUserWidget::SetParentMenu(UMainMenuWidget* InParentMenu)
{
	ParentMenu = InParentMenu;
}

void UJoinUserWidget::RefreshMenu()
{
	ShowJoinButton(false);
	SearchForLanGames();
}

void UJoinUserWidget::BackButtonClicked()
{
	if (ParentMenu)
	{
		ParentMenu->ShowModeSelectionWidget();
	}
}

void UJoinUserWidget::FindGamesButtonClicked()
{
	SearchForLanGames();
}

void UJoinUserWidget::JoinGameButtonClicked()
{
	if (LastSelectedItem && SessionListView)
	{
		int32 ItemIndex = SessionListView->GetIndexForItem(LastSelectedItem);
		URelicRunnersGameInstance* GameInstance = Cast<URelicRunnersGameInstance>(GetGameInstance());
		if (GameInstance)
		{
			GameInstance->JoinGame(ItemIndex);
		}
	}
}

void UJoinUserWidget::SearchForLanGames()
{
	URelicRunnersGameInstance* GameInstance = Cast<URelicRunnersGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		if (FindGames && SessionListView)
		{
			FindGames->SetIsEnabled(false);
			SessionListView->ClearListItems();
			SessionListView->RequestRefresh();
			GameInstance->FindGames(this);
			if (SearchProgress)
			{
				SearchProgress->SetVisibility(ESlateVisibility::Visible);
			}
			ShowJoinButton(false);
		}
	}
}

void UJoinUserWidget::OnItemClicked(UObject* ClickedItem)
{
	if (SessionListView == nullptr || ClickedItem == nullptr)
	{
		return;
	}

	if (LastSelectedItem)
	{
		if (USessionListItemWidget* LastWidget = Cast<USessionListItemWidget>(SessionListView->GetEntryWidgetFromItem(LastSelectedItem)))
		{
			LastWidget->SetItemSelected(false);
		}
	}
	if (USessionListItemWidget* SelectedWidget = Cast<USessionListItemWidget>(SessionListView->GetEntryWidgetFromItem(ClickedItem)))
	{
		SelectedWidget->SetItemSelected(true);
		LastSelectedItem = ClickedItem;
		ShowJoinButton(true);
	}
}

void UJoinUserWidget::ShowJoinButton(bool IsEnabled)
{
	if (JoinButton)
	{
		if (IsEnabled)
		{
			JoinButton->SetIsEnabled(true);
			JoinButton->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			JoinButton->SetIsEnabled(false);
			JoinButton->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UJoinUserWidget::OnFindSessionsComplete(FString Str)
{
	USessionListItemData* NewSessionItem = NewObject<USessionListItemData>();
	if (NewSessionItem)
	{
		NewSessionItem->SessionName = Str;
	}

	if (SessionListView)
	{
		SessionListView->AddItem(NewSessionItem);
	}

	EnableRefresh();
}

void UJoinUserWidget::EnableRefresh()
{
	if (FindGames)
	{
		FindGames->SetIsEnabled(true);
	}

	if (SearchProgress)
	{
		SearchProgress->SetVisibility(ESlateVisibility::Hidden);
	}
}