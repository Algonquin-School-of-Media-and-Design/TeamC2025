// Fill out your copyright notice in the Description page of Project Settings.


#include "JoinUserWidget.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "RelicRunners/Game/RelicRunnersGameInstance.h"
#include "Components/Button.h"
#include "Components/TileView.h"
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

	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &UJoinUserWidget::JoinGameButtonClicked);
	}

	if (SessionTileView)
	{
		SessionTileView->OnEntryWidgetGenerated().AddUObject(this, &UJoinUserWidget::HandleEntryGenerated);
	}

}

void UJoinUserWidget::HandleEntryGenerated(UUserWidget& EntryWidget)
{
	if (USessionListItemWidget* SessionWidget = Cast<USessionListItemWidget>(&EntryWidget))
	{
		SessionWidget->OnSessionClicked.BindUObject(this, &UJoinUserWidget::HandleSessionClicked);
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
	if (LastSelectedItem && SessionTileView)
	{
		int32 ItemIndex = SessionTileView->GetIndexForItem(LastSelectedItem);
		URelicRunnersGameInstance* GameInstance = Cast<URelicRunnersGameInstance>(GetGameInstance());
		if (GameInstance)
		{
			GameInstance->JoinGame(ItemIndex);
		}
	}
}

void UJoinUserWidget::HandleSessionClicked(USessionListItemData* ClickedSession)
{
	LastSelectedItem = ClickedSession;
	ShowJoinButton(true);

	// Reset all items to white
	if (SessionTileView)
	{
		for (UObject* Item : SessionTileView->GetListItems())
		{
			if (USessionListItemWidget* Entry = Cast<USessionListItemWidget>(SessionTileView->GetEntryWidgetFromItem(Item)))
			{
				Entry->SetSelected(Item == LastSelectedItem);
			}
		}
	}
}



void UJoinUserWidget::SearchForLanGames()
{
	URelicRunnersGameInstance* GameInstance = Cast<URelicRunnersGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		if (FindGames && SessionTileView)
		{
			FindGames->SetIsEnabled(false);
			SessionTileView->ClearListItems();
			SessionTileView->RequestRefresh();
			GameInstance->FindGames(this);
			if (SearchProgress)
			{
				SearchProgress->SetVisibility(ESlateVisibility::Visible);
			}
			ShowJoinButton(false);
		}
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

	if (SessionTileView)
	{
		SessionTileView->AddItem(NewSessionItem);

		// After adding, try to get its entry widget
		if (USessionListItemWidget* Entry = Cast<USessionListItemWidget>(SessionTileView->GetEntryWidgetFromItem(NewSessionItem)))
		{
			Entry->OnSessionClicked.BindUObject(this, &UJoinUserWidget::HandleSessionClicked);
		}
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