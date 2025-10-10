// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnZone.h"
#include <NavigationSystem.h>
#include <NavMesh/NavMeshBoundsVolume.h>


FVector ASpawnZone::GetRandomPosition()
{
	FVector origin = GetActorLocation();
	float radiusToUse = m_radius;

	if (SpawningBounds != nullptr)
	{
		FVector boxExtent;
		SpawningBounds->GetActorBounds(false, origin, boxExtent);
		radiusToUse = boxExtent.GetMax();
	}

	FNavLocation RandomPoint;
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());

	if (NavSys->GetRandomPointInNavigableRadius(origin, radiusToUse, RandomPoint))
	{
		return RandomPoint.Location;
	}

	UE_LOG(LogTemp, Error, TEXT("could not find random spawn point on navmesh"));

	return FVector();
}
