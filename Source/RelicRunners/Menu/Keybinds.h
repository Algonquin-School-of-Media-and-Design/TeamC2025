// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class RELICRUNNERS_API Keybinds
{
public:
	Keybinds();
	~Keybinds();

	struct FKeybindings
	{
		FString Name;
		FKey Bind;
	};

	TArray<FKeybindings> KeyBinds;
};
