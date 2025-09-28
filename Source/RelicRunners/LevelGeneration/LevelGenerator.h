// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelGenerator.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFloorCheckNeighbour);

UCLASS()
class RELICRUNNERS_API ALevelGenerator : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALevelGenerator();

	UPROPERTY(BlueprintAssignable)
	FFloorCheckNeighbour FloorCheckNeighbour;

	UPROPERTY(EditAnywhere, Category = "SpawningValues")
	TSubclassOf<class AGeneratedFloor> GeneratedFloorTemplate;

	UPROPERTY(EditAnywhere, Category = "SpawningValues")
	TSubclassOf<class AGeneratedObstacle> GeneratedObstacleTemplate;

	UPROPERTY(EditAnywhere, Category = "SpawningValues", meta = (ClampMin = "1", UIMin = "1"))
	int SpawnWidth;

	UPROPERTY(EditAnywhere, Category = "SpawningValues", meta = (ClampMin = "1", UIMin = "1"))
	int SpawnDepth;

	UPROPERTY(EditAnywhere, Category = "SpawningValues", meta = (ClampMin = "0", UIMin = "0", ClampMax = "100", UIMax = "100"))
	float FullPercentage;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	void GenerateFloor(int x, int y);

	int GetMapWidth() {return SpawnWidth;}
	int GetMapDepth() {return SpawnDepth;}

	float GetFullPercentage() { return FullPercentage; }

	class AGeneratedFloor* GetGeneratedFloorAtIndex(int index) { return GeneratedFloorArray[index]; }

private:
	TArray<class AGeneratedFloor*> GeneratedFloorArray;
	TArray<class AGeneratedObstacle*> GeneratedObstacleArray;
};
