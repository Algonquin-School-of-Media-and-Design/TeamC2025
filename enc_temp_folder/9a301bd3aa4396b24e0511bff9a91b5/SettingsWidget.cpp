// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingsWidget.h"
#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/TileView.h"
#include "KeybindingsListData.h"

void USettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &USettingsWidget::BackButtonClicked);
	}

	// Initialize and populate keybindings
	InitializeDefaultKeybindings();

	if (KeybindingsTileView)
	{
		KeybindingsTileView->ClearListItems();

		for (UKeybindingsListData* Binding : DefaultKeybindings)
		{
			KeybindingsTileView->AddItem(Binding);
		}
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
			NewEntry->Keybind = Key.GetDisplayName().ToString();
			DefaultKeybindings.Add(NewEntry);
		};

	AddBinding("Inventory", EKeys::I);
	AddBinding("Upgrades", EKeys::U);
	AddBinding("Interact", EKeys::E);
	AddBinding("Map", EKeys::M);
	AddBinding("Pause / Menu", EKeys::Escape);
}