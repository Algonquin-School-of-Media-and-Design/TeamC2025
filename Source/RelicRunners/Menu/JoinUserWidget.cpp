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
#include "Components/Border.h"
#include "MainMenuWidget.h"
#include "Components/CircularThrobber.h"
#include <Kismet/GameplayStatics.h>
#include "ModeSelectionWidget.h"

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
	UWorld* World = GetWorld();
	if (World)
	{
		// Travel into your gameplay level
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundActors);

		AActor* MainMenuCamera = nullptr;
		for (auto actor : FoundActors)
		{
			if (actor->ActorHasTag("MainMenuCamera"))
			{
				MainMenuCamera = actor;
			}
		}

		if (MainMenuCamera)
		{
			World->GetFirstPlayerController()->SetViewTargetWithBlend(MainMenuCamera, 0.5f);
		}

		if (ParentMenu)
		{
			ParentMenu->ShowModeSelectionWidget();
			ParentMenu->GetModeSelectionWidget()->ResetUI();
		}

		URelicRunnersGameInstance* GameInstance = Cast<URelicRunnersGameInstance>(GetGameInstance());
		GameInstance->LeaveSession();
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
	if (!IsValid(this) || !SessionTileView) return;

	URelicRunnersGameInstance* GameInstance = Cast<URelicRunnersGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		if (FindGames && SessionTileView)
		{
			FindGames->SetIsEnabled(false);
			SessionTileView->ClearListItems();
			SessionTileView->RequestRefresh();
			SessionsBorder->SetVisibility(ESlateVisibility::Collapsed);
			GameInstance->FindGames(this);
			if (RefreshProgress)
			{
				RefreshProgress->SetVisibility(ESlateVisibility::Visible);
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
			JoinBorder->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			JoinButton->SetIsEnabled(false);
			JoinButton->SetVisibility(ESlateVisibility::Hidden);
			JoinBorder->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UJoinUserWidget::OnFindSessionsComplete(FString Str)
{
	if (!IsValid(this) || !SessionTileView) return;

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
		if (SessionTileView->GetNumItems() == 0)
		{
			SessionsBorder->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			SessionsBorder->SetVisibility(ESlateVisibility::Visible);
		}
	}

	EnableRefresh();
}


void UJoinUserWidget::EnableRefresh()
{
	if (!IsValid(this) || !SessionTileView) return;

	if (FindGames)
	{
		FindGames->SetIsEnabled(true);
		RefreshProgress->SetVisibility(ESlateVisibility::Visible);
	}

	if (RefreshProgress)
	{
		RefreshProgress->SetVisibility(ESlateVisibility::Hidden);
	}
}