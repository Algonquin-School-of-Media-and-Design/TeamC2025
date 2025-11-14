// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Navigation/PathFollowingComponent.h"
#include "BTTask_SurroundTarget.generated.h"

class APawn;
class AAIController;
class UBlackboardComponent;

UCLASS()
class RELICRUNNERS_API UBTTask_SurroundTarget : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Behaviour")
	float SeparationDistanceBetweenTarget = 150;
	UPROPERTY(EditAnywhere, Category = "Behaviour")
	float SeparationDistanceBetweenFriendly = 150;
	UPROPERTY(EditAnywhere, Category = "Behaviour")
	float MoveToAcceptanceRadius = 10.f;
	UPROPERTY(EditAnywhere, Category = "Behaviour")
	TArray<TEnumAsByte<EObjectTypeQuery>> FriendlyObjectTypes;
	UPROPERTY(EditAnywhere, Category = "Behaviour")
	FVector NavExtent = FVector(300.f, 300.f, 300.f);

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey; // For an Actor or Object Key

	APawn* ControlledPawn;
	AAIController* OwnerController;
	UBlackboardComponent* BlackBoard;
	UBehaviorTreeComponent* OwnerComponent;

protected:
	//find the intentional spot for the enemy to stand near the player at
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	UFUNCTION()
	//check to see if the enemy is overlapping with other enemies and if so the enemy will find a spot to move away from other enemies adjusting for separtion distance
	void OnSeparationLocationReached(FAIRequestID RequestID, EPathFollowingResult::Type Result);
	UFUNCTION()
	//to be called once the move order from OnSeparationLocationReached is done
	void OnFinishedMoving(FAIRequestID RequestID, EPathFollowingResult::Type Result);
};
