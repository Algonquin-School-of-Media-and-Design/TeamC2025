// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Navigation/PathFollowingComponent.h"
#include "BTTask_SurroundTarget.generated.h"

/*
To Do:
- Make sure the task works with moving targets
- SphereOverlapActors is expensive, to improve preformance it would be better to know all the enemies targetting the player and use circule math for lack of a better turm, but a component would have to be add to player that would keep track of all enemies targeting in order to properly form the circlue
*/

class APawn;
class AAIController;
class UBlackboardComponent;

UCLASS()
class RELICRUNNERS_API UBTTask_SurroundTarget : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Behaviour")
	float SeparationDistanceBetweenTarget = 150.f;
	UPROPERTY(EditAnywhere, Category = "Behaviour")
	float SeparationDistanceBetweenFriendly = 100.f;
	UPROPERTY(EditAnywhere, Category = "Behaviour")
	float MoveToAcceptanceRadius = 10.f;
	UPROPERTY(EditAnywhere, Category = "Behaviour")
	float RepathDistance = 50.f;
	UPROPERTY(EditAnywhere, Category = "Behaviour")
	TArray<TEnumAsByte<EObjectTypeQuery>> FriendlyObjectTypes;
	UPROPERTY(EditAnywhere, Category = "Behaviour")
	FVector NavExtent = FVector(300.f, 300.f, 300.f);

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey; // For an Actor or Object Key

	FVector LastTargetLocation;
	bool pathWasChanged = false;

	APawn* ControlledPawn;
	AAIController* OwnerController;
	UBlackboardComponent* BlackBoard;
	UBehaviorTreeComponent* OwnerComponent;
	AActor* Target;

protected:
	UBTTask_SurroundTarget();
	//find the intentional spot for the enemy to stand near the player at
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	//incase the node get abourt for what ever resone the delegates need to be cleared
	EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	//this is here so we can check if we need to update the move to location
	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	UFUNCTION()
	void finishTask(bool result);
	UFUNCTION()
	//to be called once the move order from OnSeparationLocationReached is done
	void OnFinishedMoving(FAIRequestID RequestID, EPathFollowingResult::Type Result);
	UFUNCTION()
	//if does what the names says, if you can't read it then that on you
	bool FindSurroundLocation(FVector& result);
	UFUNCTION()
	EPathFollowingRequestResult::Type SetUpPawnMoveTo();
};
