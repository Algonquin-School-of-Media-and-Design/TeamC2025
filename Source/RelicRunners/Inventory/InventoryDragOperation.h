// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Blueprint/WidgetBlueprintLibrary.h>
#include "InventoryDragOperation.generated.h"

UCLASS()
class RELICRUNNERS_API UInventoryDragOperation : public UDragDropOperation
{
	GENERATED_BODY()
	
public:

	enum Locations
	{
		Shop,
		Inventory,
		Forge,
		Equipped
	};

	Locations FromWhere;
};
