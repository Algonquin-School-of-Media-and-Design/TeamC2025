// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_SurroundTarget.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"

UBTTask_SurroundTarget::UBTTask_SurroundTarget()
{
	//this make so their is a instance of this node for the behavoir tree. This make it safer to bind the delgates to this node for a few reason but with out it unreal occisually crash when binding delegate. This most likely because node can be share amoung multiple AIs
	bCreateNodeInstance = true;
	bNotifyTick = true;
}

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

	
	Target = Cast<AActor>(BlackBoard->GetValueAsObject(TargetActorKey.SelectedKeyName));

	//This may seem like a reduntaded check but I already did this to my self once and on the first test non the less
	if (Target == Cast<AActor>(ControlledPawn))
	{
		UE_LOG(LogTemp, Warning, TEXT("Target actor is the same as controlled pawn in surrounding target task"));
		return EBTNodeResult::Failed;
	}
	else if (!Target)
	{
		return EBTNodeResult::Failed;
	}

	LastTargetLocation = Target->GetActorLocation();

	OwnerController->StopMovement();

	if (SetUpPawnMoveTo() == EPathFollowingRequestResult::RequestSuccessful)
	{
		OwnerController->ReceiveMoveCompleted.AddDynamic(this, &UBTTask_SurroundTarget::OnFinishedMoving);
		return EBTNodeResult::InProgress;
	}

	return EBTNodeResult::Failed;
}

void UBTTask_SurroundTarget::OnFinishedMoving(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (Result == EPathFollowingResult::Success)
	{
		finishTask(true);
	}
	else if(Result == EPathFollowingResult::Aborted && !pathWasChanged)
	{
		finishTask(false);
	}
	else
	{
		pathWasChanged = false;
	}
}

bool UBTTask_SurroundTarget::FindSurroundLocation(FVector& result)
{
	if (!OwnerController || !BlackBoard || !ControlledPawn)
	{
		return false;
	}

	FVector targetLocation = Target->GetActorLocation() + ((ControlledPawn->GetActorLocation() - Target->GetActorLocation()).GetSafeNormal() * SeparationDistanceBetweenTarget);
	FVector worldMoveLocation = targetLocation;

	TArray<AActor*> overlappingActors;
	TArray<AActor*> ignoredActors;
	ignoredActors.Add(ControlledPawn);
	ignoredActors.Add(Target);

	bool sphereResult = UKismetSystemLibrary::SphereOverlapActors(GetWorld(), targetLocation, SeparationDistanceBetweenFriendly, FriendlyObjectTypes, nullptr, ignoredActors, overlappingActors);

	if (sphereResult || overlappingActors.Num() > 0)
	{
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

			float distance = FVector::Dist(actor->GetActorLocation(), targetLocation);

			if (distance > 0.f)
			{
				float weight = 1.f / distance;
				FVector direction = (ControlledPawn->GetActorLocation() - actor->GetActorLocation()).GetSafeNormal();

				moveDirection += direction * weight;
				totalWeights += weight;

				closestDistance = FMath::Min(closestDistance, distance);
			}
		}

		if (!moveDirection.IsNearlyZero())
		{
			moveDirection /= totalWeights;
			worldMoveLocation = (moveDirection.GetSafeNormal() * FMath::Max(0, (SeparationDistanceBetweenFriendly - closestDistance))) + ControlledPawn->GetActorLocation();
		}
	}

	result = worldMoveLocation;

	return true;
}

EPathFollowingRequestResult::Type UBTTask_SurroundTarget::SetUpPawnMoveTo()
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	FVector worldMoveLocation;
	FNavLocation moveLocation;
	FAIMoveRequest requestLocation;
	FNavPathSharedPtr navPath;

	if (!FindSurroundLocation(worldMoveLocation) || !NavSys->ProjectPointToNavigation(worldMoveLocation, moveLocation, NavExtent))
	{
		return EPathFollowingRequestResult::Type::Failed;
	}

	requestLocation.SetGoalLocation(moveLocation);
	requestLocation.SetAcceptanceRadius(MoveToAcceptanceRadius);

	return OwnerController->MoveTo(requestLocation, &navPath);;
}

EBTNodeResult::Type UBTTask_SurroundTarget::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (OwnerController)
	{
		OwnerController->ReceiveMoveCompleted.RemoveDynamic(this, &UBTTask_SurroundTarget::OnFinishedMoving);
	}
	
	return EBTNodeResult::Aborted;
}

void UBTTask_SurroundTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!OwnerController || !BlackBoard || !ControlledPawn || !Target)
	{
		finishTask(false);
		return;
	}

	if (FVector::Dist(LastTargetLocation, Target->GetActorLocation()) > RepathDistance)
	{
		LastTargetLocation = Target->GetActorLocation();

		OwnerController->StopMovement();

		if (SetUpPawnMoveTo() != EPathFollowingRequestResult::RequestSuccessful)
		{
			finishTask(false);
			return;
		}

		pathWasChanged = true;
	}
}

void UBTTask_SurroundTarget::finishTask(bool result)
{
	if (OwnerController)
	{
		OwnerController->ReceiveMoveCompleted.RemoveDynamic(this, &UBTTask_SurroundTarget::OnFinishedMoving);
	}

	if (result)
	{
		FinishLatentTask(*OwnerComponent, EBTNodeResult::Succeeded);
	}
	else
	{
		FinishLatentTask(*OwnerComponent, EBTNodeResult::Failed);
	}
}
