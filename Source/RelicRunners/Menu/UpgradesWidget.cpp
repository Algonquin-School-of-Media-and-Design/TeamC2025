// Fill out your copyright notice in the Description page of Project Settings.


#include "UpgradesWidget.h"
#include "MainMenuWidget.h"
#include "Components/Button.h"

void UUpgradesWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &UUpgradesWidget::BackButtonClicked);
	}
}

void UUpgradesWidget::SetParentMenu(UMainMenuWidget* InParentMenu)
{
	ParentMenu = InParentMenu;
}

void UUpgradesWidget::BackButtonClicked()
{
	if (ParentMenu)
	{
		ParentMenu->ShowModeSelectionWidget();
	}
}