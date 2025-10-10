// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpawnPoint.h"
#include "SpawnZone.generated.h"

/**
 * 
 */

class ANavMeshBoundsVolume;

UCLASS()
class RELICRUNNERS_API ASpawnZone : public ASpawnPoint
{
	GENERATED_BODY()
	
	public:
		virtual FVector GetRandomPosition() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Point Properties", meta = (AllowPrivateAccess = "true"))
	ANavMeshBoundsVolume* SpawningBounds;
};
