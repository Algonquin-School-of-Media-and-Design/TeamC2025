// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelGenerator.h"
#include "GeneratedFloor.h"

// Sets default values
ALevelGenerator::ALevelGenerator():
	GeneratedFloorTemplate(nullptr),
	GeneratedObstacleTemplate(nullptr),
	SpawnWidth(1),
	SpawnDepth(1),
	FullPercentage(75.0f)

{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ALevelGenerator::BeginPlay()
{
	Super::BeginPlay();
	
	GeneratedFloorArray.Empty();

	for (int y = 0; y < SpawnDepth; y++)
	{
		for (int x = 0; x < SpawnWidth; x++)
		{
			GenerateFloor(x, y);
		}
	}

	FloorCheckNeighbour.Broadcast();

}

void ALevelGenerator::GenerateFloor(int x, int y)
{
	if (GeneratedFloorTemplate == nullptr)
		return;

	if (GeneratedFloorArray.Num() == x + (SpawnWidth * y))
	{
		FVector positionOffset = FVector(x, y, 0.0f);
		AGeneratedFloor* floorToGenerate = GetWorld()->SpawnActor<AGeneratedFloor>(GeneratedFloorTemplate, GetActorLocation() + positionOffset, FRotator(0.0f, 0.0f, 0.0f));
		floorToGenerate->InitializeFloor(this, x ,y);

		GeneratedFloorArray.Add(floorToGenerate);
	}
}