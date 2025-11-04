// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "KeybindingsListData.generated.h"

/**
 * 
 */
UCLASS()
class RELICRUNNERS_API UKeybindingsListData : public UObject
{
	GENERATED_BODY()
public:
	FString Name;
	FString Keybind;
};
