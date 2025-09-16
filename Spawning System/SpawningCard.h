#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SpawningCard.generated.h"

UCLASS()
class QUESTIONABLE_030_API USpawnCard : public UDataAsset
{
	GENERATED_BODY()
public:
	//et default values
	USpawnCard();

public:
	//The cost to spawn the enemy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ClampMin = "0.1"))
	float m_spawnCost;
	//How likely it is to spawn the enemy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ClampMin = "0.1"))
	float m_spawnWeight;
	//the enemy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<APawn> m_enemyType;
};
