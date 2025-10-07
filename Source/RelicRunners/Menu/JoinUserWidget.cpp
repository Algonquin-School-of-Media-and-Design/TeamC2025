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
#include "Components/ProgressBar.h"
#include "MainMenuWidget.h"
#include "Components/CircularThrobber.h"
#include <Kismet/GameplayStatics.h>
#include "ModeSelectionWidget.h"
#include "GameFramework/GameStateBase.h"
#include "RelicRunners/Classes/ClassData.h"
#include "RelicRunners/Classes/ClassInfo.h"
#include <RelicRunners/PlayerController/RelicRunnersPlayerController.h>

void UJoinUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SessionTileView)
	{
		SessionTileView->OnEntryWidgetGenerated().AddUObject(this, &UJoinUserWidget::HandleEntryGenerated);
	}

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

	if (StartButton)
	{
		if (const UWorld* World = GetWorld())
		{
			//Only show for the listen server / host
			bool bIsHost = World->GetAuthGameMode() != nullptr;
			StartButton->SetVisibility(bIsHost ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		}

		StartButton->OnClicked.AddDynamic(this, &UJoinUserWidget::StartGameButtonClicked);
	}

	if (AresButton)
	{
		AresButton->OnClicked.AddDynamic(this, &UJoinUserWidget::OnAresClicked);
	}

	if (ArtemisButton)
	{
		ArtemisButton->OnClicked.AddDynamic(this, &UJoinUserWidget::OnArtemisClicked);
	}

	if (AphroditeButton)
	{
		AphroditeButton->OnClicked.AddDynamic(this, &UJoinUserWidget::OnAphroditeClicked);
	}

	if (NemesisButton)
	{
		NemesisButton->OnClicked.AddDynamic(this, &UJoinUserWidget::OnNemesisClicked);
	}

	UpdateFindGamesButtonVisibility();
	SetSelectedClass("Ares");
}

void UJoinUserWidget::SetButtonColor(UButton* Button, const FLinearColor& Color)
{
	if (!Button) return;

	Button->SetBackgroundColor(Color);
}

void UJoinUserWidget::OnAresClicked() { SetSelectedClass("Ares"); }
void UJoinUserWidget::OnArtemisClicked() { SetSelectedClass("Artemis"); }
void UJoinUserWidget::OnAphroditeClicked() { SetSelectedClass("Aphrodite"); }
void UJoinUserWidget::OnNemesisClicked() { SetSelectedClass("Nemesis"); }

void UJoinUserWidget::SetSelectedClass(FName ClassKey)
{
	FLinearColor Gold(1, 0.5f, 0, 1.f);
	FLinearColor Cyan(0.f, 1.f, 1.f, 1.f);

	// Reset all buttons to cyan
	SetButtonColor(AresButton, Cyan);
	SetButtonColor(ArtemisButton, Cyan);
	SetButtonColor(AphroditeButton, Cyan);
	SetButtonColor(NemesisButton, Cyan);

	// Highlight the selected one gold
	if (ClassKey == "Ares") SetButtonColor(AresButton, Gold);
	else if (ClassKey == "Artemis") SetButtonColor(ArtemisButton, Gold);
	else if (ClassKey == "Aphrodite") SetButtonColor(AphroditeButton, Gold);
	else if (ClassKey == "Nemesis") SetButtonColor(NemesisButton, Gold);
	
	if (!ClassDataAsset || !ClassDataAsset->Classes.Contains(ClassKey))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid class key: %s"), *ClassKey.ToString());
		return;
	}

	const FClassData& Info = ClassDataAsset->Classes[ClassKey];
	const FClassData MaxValues = ClassDataAsset->GetMaxStats();

	// Text updates
	if (TB_ClassName) TB_ClassName->SetText(FText::FromName(Info.ClassName));
	if (TB_ClassRole) TB_ClassRole->SetText(FText::FromString(Info.Role));
	if (TB_Ability1)  TB_Ability1->SetText(FText::FromString(Info.Ability1));
	if (TB_Ability2)  TB_Ability2->SetText(FText::FromString(Info.Ability2));
	if (TB_Ability3)  TB_Ability3->SetText(FText::FromString(Info.Ability3));
	if (TB_Ability4)  TB_Ability4->SetText(FText::FromString(Info.Ability4));

	// Safe normalization helper lambda
	auto Normalize = [](float Value, float Max)
		{
			return (Max > 0.f) ? (Value / Max) : 0.f;
		};

	// Normalized progress bars
	if (HealthBar)       HealthBar->SetPercent(Normalize(Info.Health, MaxValues.Health)); TB_Health->SetText(FText::FromString(FString::FromInt(Info.Health)));
	if (ArmorBar)        ArmorBar->SetPercent(Normalize(Info.Armor, MaxValues.Armor)); TB_Armor->SetText(FText::FromString(FString::FromInt(Info.Armor)));
	if (DexterityBar)    DexterityBar->SetPercent(Normalize(Info.Dexterity, MaxValues.Dexterity)); TB_Dexterity->SetText(FText::FromString(FString::FromInt(Info.Dexterity)));
	if (StrengthBar)     StrengthBar->SetPercent(Normalize(Info.Strength, MaxValues.Strength)); TB_Strength->SetText(FText::FromString(FString::FromInt(Info.Strength)));
	if (IntelligenceBar) IntelligenceBar->SetPercent(Normalize(Info.Intelligence, MaxValues.Intelligence)); TB_Intelligence->SetText(FText::FromString(FString::FromInt(Info.Intelligence)));
	if (LuckBar)         LuckBar->SetPercent(Normalize(Info.Luck, MaxValues.Luck)); TB_Luck->SetText(FText::FromString(FString::FromInt(Info.Luck)));
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
	URelicRunnersGameInstance* GameInstance = GetGameInstance<URelicRunnersGameInstance>();
	if (GameInstance)
	{
		GameInstance->BackToMainMenu();
	}
}

void UJoinUserWidget::FindGamesButtonClicked()
{
	URelicRunnersGameInstance* GameInstance = GetGameInstance<URelicRunnersGameInstance>();
	if (!GameInstance) return;

	SearchForLanGames();
}

void UJoinUserWidget::UpdateFindGamesButtonVisibility()
{
	if (!FindGames && !SessionTileView && !SessionsBorder && !JoinBorder)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UpdateFindGamesButtonVisibility] UI references not ready yet."));
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UpdateFindGamesButtonVisibility] World not valid."));
		return;
	}

	AGameStateBase* GS = World->GetGameState();
	if (!GS || GS->PlayerArray.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UpdateFindGamesButtonVisibility] GameState or PlayerArray not ready, retrying..."));

		FTimerHandle RetryHandle;
		World->GetTimerManager().SetTimer(
			RetryHandle,
			[this]() { UpdateFindGamesButtonVisibility(); },
			0.1f,
			false
		);
		return;
	}

	const int32 NumPlayers = GS->PlayerArray.Num();
	UE_LOG(LogTemp, Log, TEXT("[UpdateFindGamesButtonVisibility] NumPlayers: %d"), NumPlayers);

	// Show "Find Games" button if alone in the lobby
	if (FindGames)
	{
		const bool bAlone = (NumPlayers <= 1);
		FindGames->SetVisibility(bAlone ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		FindGames->SetIsEnabled(bAlone);
	}

	// If there are multiple players, hide join UI panels
	if (NumPlayers > 1)
	{
		if (SessionTileView)
			SessionTileView->ClearListItems();

		if (SessionsBorder)
			SessionsBorder->SetVisibility(ESlateVisibility::Collapsed);

		if (JoinBorder)
			JoinBorder->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UJoinUserWidget::JoinGameButtonClicked()
{
	if (LastSelectedItem && SessionTileView)
	{
		int32 ItemIndex = SessionTileView->GetIndexForItem(LastSelectedItem);
		URelicRunnersGameInstance* GameInstance = GetGameInstance<URelicRunnersGameInstance>();
		if (GameInstance)
		{
			GameInstance->JoinGame(ItemIndex);
		}
	}
}

void UJoinUserWidget::StartGameButtonClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
		return;

	// Ask the server (host) to start the session and travel
	if (PC->HasAuthority())
	{
		// We're already the server (listen server)
		if (URelicRunnersGameInstance* GI = PC->GetGameInstance<URelicRunnersGameInstance>())
		{
			GI->StartSessionGame();
		}
	}
	else
	{
		// Client - send request to the host
		if (ARelicRunnersPlayerController* MyPC = Cast<ARelicRunnersPlayerController>(PC))
		{
			MyPC->Server_RequestStartGame();
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