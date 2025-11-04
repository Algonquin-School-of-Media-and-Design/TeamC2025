// Fill out your copyright notice in the Description page of Project Settings.


#include "KeybindingsListWidget.h"

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

    }
}