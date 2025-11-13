// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_SurroundTarget.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"

EBTNodeResult::Type UBTTask_SurroundTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	OwnerComponent = &OwnerComp;
	OwnerController = OwnerComp.GetAIOwner(); 
	BlackBoard = OwnerComp.GetBlackboardComponent();

	if (!OwnerController || !BlackBoard)
	{
		return EBTNodeResult::Failed;
	}

	ControlledPawn = OwnerController->GetPawn();

	if (!ControlledPawn)
	{
		return EBTNodeResult::Failed;
	}

	
	AActor* target = Cast<AActor>(BlackBoard->GetValueAsObject(TargetActorKey.SelectedKeyName));

	//This may seem like a reduntaded check but I already did this to my self once and on the first test non the less
	if (target == Cast<AActor>(ControlledPawn))
	{
		UE_LOG(LogTemp, Warning, TEXT("Target actor is the same as controlled pawn in surrounding target task"));
		return EBTNodeResult::Failed;
	}

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	FNavLocation moveLocation;

	FVector targetLocation = target->GetActorLocation();
	FVector worldLocation = targetLocation + ((targetLocation - ControlledPawn->GetActorLocation()).GetSafeNormal() * SeparationDistanceBetweenTarget);

	//finding the close navmesh point to the world point (mainly if their is a wall between the enemy and target or something like that)
	if (!NavSys->ProjectPointToNavigation(worldLocation, moveLocation, NavExtent))
	{
		return EBTNodeResult::Failed;
	}

	FAIMoveRequest requestLocation;
	requestLocation.SetGoalLocation(moveLocation);
	requestLocation.SetAcceptanceRadius(MoveToAcceptanceRadius);


	FNavPathSharedPtr navPath;
	EPathFollowingRequestResult::Type moveResult = OwnerController->MoveTo(requestLocation, &navPath);

	if (moveResult == EPathFollowingRequestResult::RequestSuccessful)
	{
		OwnerController->ReceiveMoveCompleted.AddDynamic(this, &UBTTask_SurroundTarget::OnSeparationLocationReached);
		return EBTNodeResult::InProgress;
	}

	return EBTNodeResult::Failed;
}

void UBTTask_SurroundTarget::OnSeparationLocationReached(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	TArray<AActor*> overlappingActors;
	TArray<AActor*> ignoredActors;
	ignoredActors.Add(ControlledPawn);
	ignoredActors.Add(Cast<AActor>(BlackBoard->GetValueAsObject(TargetActorKey.SelectedKeyName)));

	bool sphereResult = UKismetSystemLibrary::SphereOverlapActors(GetWorld(), ControlledPawn->GetActorLocation(), SeparationDistanceBetweenFriendly, FriendlyObjectTypes, nullptr, ignoredActors, overlappingActors);

	if (!sphereResult || overlappingActors.Num() == 0)
	{
		FinishLatentTask(*OwnerComponent, EBTNodeResult::Failed);
		return;
	}
	
	//using a weighted average to find the direction to move away from other enemies with the weight being the inverse of the distance to the other enemy so that closer enemies have more influence
	float totalWeights = 0.f;
	FVector moveDirection = FVector::ZeroVector;
	float closestDistance = TNumericLimits<float>::Max();

	for (AActor* actor : overlappingActors)
	{
		if (!actor->Tags.Contains("Enemy"))
		{
			continue;
		}

		float distance = FVector::Dist(actor->GetActorLocation(), ControlledPawn->GetActorLocation());

		if (distance > 0.f)
		{
			float weight = 1.f / distance;
			FVector direction = (ControlledPawn->GetActorLocation() - actor->GetActorLocation()).GetSafeNormal();

			moveDirection += direction * weight;
			totalWeights += weight;

			if (distance < closestDistance)
			{
				closestDistance = distance;
			}
		}
	}

	if (moveDirection == FVector::ZeroVector)
	{
		FinishLatentTask(*OwnerComponent, EBTNodeResult::Failed);
		return;
	}

	moveDirection /= totalWeights;
	FVector worldMoveLocation = (moveDirection.GetSafeNormal() * FMath::Max(0, (SeparationDistanceBetweenFriendly - closestDistance))) + ControlledPawn->GetActorLocation();

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	FNavLocation moveLocation;

	if (!NavSys->ProjectPointToNavigation(worldMoveLocation, moveLocation, NavExtent))
	{
		FinishLatentTask(*OwnerComponent, EBTNodeResult::Failed);
		return;
	}

	FAIMoveRequest requestLocation;
	requestLocation.SetGoalLocation(moveLocation);

	FNavPathSharedPtr navPath;
	EPathFollowingRequestResult::Type moveResult = OwnerController->MoveTo(requestLocation, &navPath);

	if (moveResult == EPathFollowingRequestResult::Failed)
	{
		FinishLatentTask(*OwnerComponent, EBTNodeResult::Failed);
		return;
	}

	FinishLatentTask(*OwnerComponent, EBTNodeResult::Succeeded);
	OwnerController->ReceiveMoveCompleted.AddDynamic(this, &UBTTask_SurroundTarget::OnSeparationLocationReached);
}

void UBTTask_SurroundTarget::OnFinishedMoving(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	FinishLatentTask(*OwnerComponent, EBTNodeResult::Succeeded);
}
