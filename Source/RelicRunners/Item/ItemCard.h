// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemData.h"
#include "ItemCard.generated.h"


UCLASS()
class RELICRUNNERS_API UItemCard : public UDataAsset
{
	GENERATED_BODY()
public:
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemData ItemData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool RandomizeRarity = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool RandomizeStats = false;*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ItemType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UItemMeshData* MeshData;;
public:
	UItemCard();
	
};
