// Fill out your copyright notice in the Description page of Project Settings.

#include "PauseMenu.h"
#include "Components/Button.h"
#include <RelicRunners/RelicRunnersCharacter.h>
#include <RelicRunners/PlayerController/RelicRunnersPlayerController.h>
#include <RelicRunners/Game/RelicRunnersGameInstance.h>
#include "MainMenuWidget.h"
#include <RelicRunners/RelicRunnersGameMode.h>
#include "RelicRunners/Menu/SettingsWidget.h"

void UPauseMenu::NativeConstruct()
{
    Super::NativeConstruct();

    if (ResumeButton)
    {
        ResumeButton->OnClicked.AddDynamic(this, &UPauseMenu::OnResumeButtonClicked);
    }

    if (SettingsButton)
    {
        SettingsButton->OnClicked.AddDynamic(this, &UPauseMenu::OnSettingsButtonClicked);
    }

    if (RecallButton)
    {
        RecallButton->OnClicked.AddDynamic(this, &UPauseMenu::OnRecallButtonClicked);
    }

    if (QuitButton)
    {
        QuitButton->OnClicked.AddDynamic(this, &UPauseMenu::OnQuitButtonClicked);
    }
}

void UPauseMenu::OnResumeButtonClicked()
{
    ARelicRunnersCharacter* Character = Cast<ARelicRunnersCharacter>(GetOwningPlayerPawn());
    if (Character)
    {
        Character->PauseUI();
    }
}

void UPauseMenu::OnRecallButtonClicked()
{

}

void UPauseMenu::OnSettingsButtonClicked()
{
    if (!SettingsWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("PauseMenu: SettingsWidgetClass not set!"));
        return;
    }

    APlayerController* PC = GetOwningPlayer();
    if (!PC)
    {
        UE_LOG(LogTemp, Warning, TEXT("PauseMenu: No owning player!"));
        return;
    }

    if (!SettingsWidget)
    {
        SettingsWidget = CreateWidget<USettingsWidget>(PC, SettingsWidgetClass);
        if (!SettingsWidget)
        {
            UE_LOG(LogTemp, Error, TEXT("PauseMenu: Failed to create SettingsWidget!"));
            return;
        }
    }
    else
    {
        SettingsWidget->RemoveFromParent();
    }

    // Add to viewport if not already visible
    if (!SettingsWidget->IsInViewport())
    {
        SettingsWidget->AddToViewport(5);
    }

    UE_LOG(LogTemp, Log, TEXT("PauseMenu: Opened Settings Widget."));
}

void UPauseMenu::OnQuitButtonClicked()
{
    UWorld* World = GetWorld();
    if (World)
    {
        URelicRunnersGameInstance* GameInstance = Cast<URelicRunnersGameInstance>(World->GetGameInstance());
        if (GameInstance)
        {
            GameInstance->BackToMainMenu();
        }
    }
}