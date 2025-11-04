// Fill out your copyright notice in the Description page of Project Settings.


#include "KeybindingsListWidget.h"
#include "KeybindingsListData.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UKeybindingsListWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UKeybindingsListWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    UKeybindingsListData* SessionData = Cast<UKeybindingsListData>(ListItemObject);
    if (SessionData)
    {
        Setup(SessionData);
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