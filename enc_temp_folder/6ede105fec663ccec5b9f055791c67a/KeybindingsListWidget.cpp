// Fill out your copyright notice in the Description page of Project Settings.


#include "KeybindingsListWidget.h"
#include "KeybindingsListData.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "SettingsWidget.h"

void UKeybindingsListWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (!B_BindButton)
    {
        UE_LOG(LogTemp, Error, TEXT("B_BindButton is NULL in %s"), *GetName());
        return;
    }

    B_BindButton->OnClicked.AddDynamic(this, &UKeybindingsListWidget::OnBindButtonClicked);
}

void UKeybindingsListWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    UKeybindingsListData* SessionData = Cast<UKeybindingsListData>(ListItemObject);
    if (SessionData)
    {
        Setup(SessionData);
    }
    if (KeybindData)
    {
        KeybindData->BoundWidget = this;
    }
}

void UKeybindingsListWidget::Setup(UKeybindingsListData* InData)
{
    KeybindData = InData;

    if (KeybindData)
    {
        TB_Name->SetText(FText::FromString(KeybindData->Name));
        TB_Bind->SetText(FText::FromString(KeybindData->Keybind.GetFName().ToString()));
    }
}

void UKeybindingsListWidget::OnBindButtonClicked()
{
    USettingsWidget* Settings = Cast<USettingsWidget>(GetTypedOuter(USettingsWidget::StaticClass()));
    if (Settings)
    {
        Settings->StartRebinding(this); // tells parent to start listening
    }

    if (TB_Bind)
        TB_Bind->SetText(FText::FromString(TEXT("???")));
}

UKeybindingsListData* UKeybindingsListWidget::GetKeybindData() const
{
    return KeybindData;
}

void UKeybindingsListWidget::RefreshVisual()
{
    if (!KeybindData || !TB_Bind)
        return;

    // Set displayed text
    FText DisplayText = KeybindData->Keybind.IsValid() ?
        FText::FromName(KeybindData->Keybind.GetFName()) :
        FText::FromString(TEXT("None"));
    TB_Bind->SetText(DisplayText);

    // Default color
    FSlateColor Color = FSlateColor(FLinearColor::White);

        // Red if the key is invalid
    if (DisplayText.ToString() == "None")
    {
        Color = FSlateColor(FLinearColor::Red);
    }

    TB_Bind->SetColorAndOpacity(Color);
}
