/***************************************************************************************
Class Name:  USpawnCard
Author:      Thomas Johnson
Description: This Class acts more like a struct but need to be class to use UDataAsset functionality. Hold the information need to spawn enemies
Date Last Changed:        2025/03/22
****************************************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SpawningCard.generated.h"

UCLASS()
class RELICRUNNERS_API USpawnCard : public UDataAsset
{
	GENERATED_BODY()
public:
	//the default values
	USpawnCard();

public:
	//The cost to spawn the enemy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ClampMin = "0.1"))
	float SpawnCost;
	//How likely it is to spawn the enemy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ClampMin = "0.1"))
	float SpawnWeight;
	//the enemy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	//The subclass of pawn the card represents
	TSubclassOf<APawn> EnemyClass;
};
