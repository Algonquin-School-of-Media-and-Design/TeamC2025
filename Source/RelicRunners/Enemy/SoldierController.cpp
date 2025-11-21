// Fill out your copyright notice in the Description page of Project Settings.


#include "SoldierController.h"
#include <NavigationSystem.h>
#include <Perception/AIPerceptionComponent.h>

ASoldierController::ASoldierController()
{
	//set up default values
	PatrolRadius = 1000.f;
	IsInCombat = false;
	OriginPatrolZone = FVector::ZeroVector;
	CurrentPatrolCenter = FVector::ZeroVector;
	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));
}

void ASoldierController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//It is here and not just in header in chase more logic is add later 
bool ASoldierController::GetIsInCombat() const
{
	return IsInCombat;
}

bool ASoldierController::IsInPatrolRange()
{
	//null check then calculate and check distances
	if (GetPawn())
	{
		float Distance = FVector::Dist(GetPawn()->GetActorLocation(), OriginPatrolZone);
		if (Distance <= PatrolRadius)
		{
			return true;
		}
	}

	return false;
}

FVector ASoldierController::GetRandomPatrolPoint()
{
	//set up variables related to the nav mesh
	FNavLocation randomPoint;
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());

	//finding the point
	if (NavSys->GetRandomPointInNavigableRadius(OriginPatrolZone, PatrolRadius, randomPoint))
	{
		return randomPoint.Location;
	}

//error log not need if not playtest build
#if WITH_EDITOR || UE_BUILD_TEST
	UE_LOG(LogTemp, Warning, TEXT("could not find random spawn point on navmesh"));
#endif

	return FVector::ZeroVector;
}

void ASoldierController::BeginPlay()
{
	Super::BeginPlay();
	APawn* pawn = GetPawn();

	//check pawn and then set up the vectors for patrol
	if (pawn)
	{
		OriginPatrolZone = pawn->GetActorLocation();
		CurrentPatrolCenter = OriginPatrolZone;
	}
}

void ASoldierController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (StartingBehaviorTree)
	{
		RunBehaviorTree(StartingBehaviorTree);
	}
}
