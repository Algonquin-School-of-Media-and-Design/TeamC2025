// Fill out your copyright notice in the Description page of Project Settings.


#include "SoldierController.h"
#include <NavigationSystem.h>
#include <Perception/AIPerceptionComponent.h>

ASoldierController::ASoldierController()
{
	PatrolRadius = 1000.f;
	IsInCombat = false;
	CentarePatrolZone = FVector::ZeroVector;
	CurrentPatrolPoint = FVector::ZeroVector;
	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));
}

void ASoldierController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ASoldierController::GetIsInCombat() const
{
	return IsInCombat;
}

bool ASoldierController::IsInPatrolRange()
{
	if (GetPawn())
	{
		float Distance = FVector::Dist(GetPawn()->GetActorLocation(), CentarePatrolZone);
		if (Distance <= PatrolRadius)
		{
			return true;
		}
	}

	return false;
}

FVector ASoldierController::GetRandomPatrolPoint()
{
	FNavLocation randomPoint;
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());

	if (NavSys->GetRandomPointInNavigableRadius(CentarePatrolZone, PatrolRadius, randomPoint))
	{
		return randomPoint.Location;
	}

	UE_LOG(LogTemp, Warning, TEXT("could not find random spawn point on navmesh"));

	return FVector::ZeroVector;
}

FVector ASoldierController::GetCurrentPatrolPoint() const
{
	return CurrentPatrolPoint;
}

void ASoldierController::BeginPlay()
{
	Super::BeginPlay();
}
