// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelGenerator.h"
#include "GeneratedFloor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ALevelGenerator::ALevelGenerator():
	GeneratedFloorTemplate(nullptr),
	GeneratedObstacleTemplate(nullptr),
	MeshComponent(nullptr),
	SpawnWidth(1),
	SpawnDepth(1),
	FullPercentage(75.0f)

{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ISMComp = CreateDefaultSubobject<UInstancedStaticMeshComponent>("Mesh");
	RootComponent = ISMComp;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("RegularMesh");
	MeshComponent->SetupAttachment(ISMComp);

}

// Called when the game starts or when spawned
void ALevelGenerator::BeginPlay()
{
	Super::BeginPlay();

	if (ISMComp->GetStaticMesh() == nullptr)
		return;

	TArray<FTransform> transformArray;

	for (int y = 0; y < SpawnDepth; y++)
	{
		for (int x = 0; x < SpawnWidth; x++)
		{
			FVector offset;
			FVector meshExtent = ISMComp->GetStaticMesh()->GetBounds().BoxExtent;
			offset = FVector(meshExtent.X * x * 5.0f, meshExtent.Y * y * 5.0f, 0.0f);
			FTransform newTransform;
			newTransform.SetLocation(FVector(offset));

			transformArray.Add(newTransform);
		}
	}

	ISMComp->AddInstances(transformArray, false);

	ISMComp->SetStaticMesh(MeshComponent->GetStaticMesh());

	//for (int y = 0; y < SpawnDepth; y++)
	//{
	//	for (int x = 0; x < SpawnWidth; x++)
	//	{
	//		GenerateFloor(x, y);
	//	}
	//}

	//FloorCheckNeighbour.Broadcast();
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
		
		if (CenterForceFull > 0)
		{
			if (((x - CenterForceFull) >= 0 && (x + CenterForceFull) <= SpawnWidth-1)
				&& (y - CenterForceFull) >= 0 && (y + CenterForceFull) <= SpawnDepth-1)
			{
				floorToGenerate->ForceFloorState(true);
			}
		}

		if (BorderForceFull > 0)
		{
			if (((x - BorderForceFull) < 0 || (x + BorderForceFull) > SpawnWidth-1)
				|| (y - BorderForceFull) < 0 || (y + BorderForceFull) > SpawnDepth-1)

			{
				floorToGenerate->ForceFloorState(true);
			}
		}

		GeneratedFloorArray.Add(floorToGenerate);
	}
}