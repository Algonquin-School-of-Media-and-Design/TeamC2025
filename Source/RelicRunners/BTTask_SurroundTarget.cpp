// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_SurroundTarget.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"



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

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	FAIMoveRequest requestLocation;
	AActor* target = Cast<AActor>(BlackBoard->GetValueAsObject(TargetActorKey.SelectedKeyName));
	FNavLocation moveLocation;

	FVector worldLocation = target->GetActorLocation() + (target->GetActorLocation() - ControlledPawn->GetActorLocation()).GetSafeNormal() * SeparationDistance;

	//finding the close navmesh point to the world point (mainly if their is a wall between the enemy and target or something like that)
	if (!NavSys->ProjectPointToNavigation(worldLocation, moveLocation, NavExtent))
	{
		return EBTNodeResult::Failed;
	}

	requestLocation.SetGoalLocation(moveLocation);

	FNavPathSharedPtr navPath;
	EPathFollowingRequestResult::Type moveResult = OwnerController->MoveTo(requestLocation, &navPath);

	if (moveResult == EPathFollowingRequestResult::Failed)
	{
		return EBTNodeResult::Failed;
	}

	OwnerController->ReceiveMoveCompleted.AddDynamic(this, &UBTTask_SurroundTarget::OnSeparationLocationReached);

	return EBTNodeResult::InProgress;
}

void UBTTask_SurroundTarget::OnSeparationLocationReached(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	FinishLatentTask(*OwnerComponent, EBTNodeResult::Succeeded);
}


void UBTTask_SurroundTarget::OnFinishedMoving(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
}
