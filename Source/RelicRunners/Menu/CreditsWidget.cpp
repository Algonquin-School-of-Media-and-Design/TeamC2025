// Fill out your copyright notice in the Description page of Project Settings.


#include "CreditsWidget.h"
#include "MainMenuWidget.h"
#include "Components/Button.h"

void UCreditsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &UCreditsWidget::BackButtonClicked);
	}
}

void UCreditsWidget::SetParentMenu(UMainMenuWidget* InParentMenu)
{
	ParentMenu = InParentMenu;
}

void UCreditsWidget::BackButtonClicked()
{
	if (ParentMenu)
	{
		ParentMenu->ShowModeSelectionWidget();
	}
}
