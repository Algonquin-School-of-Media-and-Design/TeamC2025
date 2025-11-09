// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingsWidget.h"
#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/TileView.h"
#include "KeybindingsListData.h"
#include <EnhancedInputSubsystems.h>
#include "Keybinds.h"
#include "KeybindingsListWidget.h"
#include <RelicRunners/PlayerController/RelicRunnersPlayerController.h>
#include <Kismet/GameplayStatics.h>
#include "RelicRunners/Game/RelicRunnersGameInstance.h"

void USettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &USettingsWidget::BackButtonClicked);
	}

	if (B_RestoreDefaults)
	{
		B_RestoreDefaults->OnClicked.AddDynamic(this, &USettingsWidget::OnRestoreDefaultsClicked);
	}
}

void USettingsWidget::OnRestoreDefaultsClicked()
{
	URelicRunnersGameInstance* GI = Cast<URelicRunnersGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->ResetToDefaults();
	}

	InitializeDefaultKeybindings();
}

void USettingsWidget::StartRebinding(UKeybindingsListWidget* Entry)
{
	WaitingForKeyEntry = Entry;
}

FReply USettingsWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (!WaitingForKeyEntry)
		return Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	FKey PressedKey = InKeyEvent.GetKey();
	if (!PressedKey.IsValid())
		return FReply::Handled();

	return HandleKeyBindPressed(PressedKey);
}

FReply USettingsWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!WaitingForKeyEntry)
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	// Use the mouse button that was pressed
	FKey PressedKey = InMouseEvent.GetEffectingButton();
	if (!PressedKey.IsValid())
		return FReply::Handled();

	return HandleKeyBindPressed(PressedKey);
}

FReply USettingsWidget::HandleKeyBindPressed(FKey PressedKey)
{
	// Get the data object for the row we’re rebinding
	UKeybindingsListData* Data = WaitingForKeyEntry->GetKeybindData();
	if (!Data)
		return FReply::Handled();

	// First, find all other bindings that use this key
	for (UKeybindingsListData* Binding : DefaultKeybindings)
	{
		if (Binding != Data && Binding->Keybind == PressedKey)
		{
			Binding->Keybind = EKeys::Invalid;
		}
	}

	// Assign new key
	Data->Keybind = PressedKey;

	// Update PlayerController keybinds
	URelicRunnersGameInstance* GI = Cast<URelicRunnersGameInstance>(GetGameInstance());
	if (GI)
	{
		if (UKeybinds* Keys = GI->Keys)
		{
			for (auto& Bind : Keys->KeyBinds)
			{
				if (Bind.Name != Data->Name && Bind.Bind == PressedKey)
					Bind.Bind = EKeys::Invalid;

				if (Bind.Name == Data->Name)
					Bind.Bind = PressedKey;
			}
		}
	}
	UpdateBinds();

	// Refresh all visuals
	for (UKeybindingsListData* Binding : DefaultKeybindings)
	{
		if (Binding->BoundWidget)
		{
			Binding->BoundWidget->RefreshVisual();
		}
	}

	// Stop listening
	WaitingForKeyEntry = nullptr;

	return FReply::Handled();
}

void USettingsWidget::UpdateBinds()
{
	// Apply changes to input system
	URelicRunnersGameInstance* GI = Cast<URelicRunnersGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->ApplyKeybindings();
	}
}

void USettingsWidget::SetParentMenu(UMainMenuWidget* InParentMenu)
{
	ParentMenu = InParentMenu;
}

void USettingsWidget::BackButtonClicked()
{
	if (ParentMenu)
	{
		ParentMenu->ShowModeSelectionWidget();
	}
}

void USettingsWidget::InitializeDefaultKeybindings()
{
	DefaultKeybindings.Empty();

	auto AddBinding = [&](const FString& ActionName, const FKey& Key)
		{
			UKeybindingsListData* NewEntry = NewObject<UKeybindingsListData>(this);
			NewEntry->Name = ActionName;
			NewEntry->Keybind = Key;
			DefaultKeybindings.Add(NewEntry);
		};

	URelicRunnersGameInstance* GI = Cast<URelicRunnersGameInstance>(GetGameInstance());
	if (!GI) return;

	for (auto Binds : GI->Keys->KeyBinds)
	{
		AddBinding(Binds.Name, Binds.Bind);
	}

	if (KeybindingsTileView)
	{
		KeybindingsTileView->ClearListItems();

		for (UKeybindingsListData* Binding : DefaultKeybindings)
		{
			KeybindingsTileView->AddItem(Binding);
		}
	}
}

void USettingsWidget::ToggleInvertedXMouse()
{
	URelicRunnersGameInstance* GI = Cast<URelicRunnersGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->Keys->InvertedXMouse = !GI->Keys->InvertedXMouse;
	}
}

void USettingsWidget::ToggleInvertedYMouse()
{
	URelicRunnersGameInstance* GI = Cast<URelicRunnersGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->Keys->InvertedYMouse = !GI->Keys->InvertedYMouse;
	}
}
