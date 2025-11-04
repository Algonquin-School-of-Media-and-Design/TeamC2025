// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "KeybindingsListData.h"
#include "KeybindingsListWidget.generated.h"

/**
 * 
 */
UCLASS()
class RELICRUNNERS_API UKeybindingsListWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	void Setup(UKeybindingsListData* InData);
private:
	UKeybindingsListData* KeybindData;
};
