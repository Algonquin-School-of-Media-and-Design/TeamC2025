// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SoldierController.generated.h"

class UAIPerceptionComponent;


UCLASS()
class RELICRUNNERS_API ASoldierController : public AAIController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//The preception component that handles detecting targets for the soldier, like the player for the enemies
	UAIPerceptionComponent* AIPerceptionComp;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	//A bool that keep track of whether the soldier 
	bool IsInCombat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//The radius the enemy will patrol around the patrol point
	float PatrolRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//The origin the control will use to find the navmesh to use
	FVector OriginPatrolZone;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//The point that the soldier will patrol around
	FVector CurrentPatrolCenter;
public:
	//The default constructor
	ASoldierController();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//The getter for IsInCombat
	bool GetIsInCombat() const;

	//A check to make sure the owned pawn is still with in the range of patrol around the patrol point
	bool IsInPatrolRange();
	//Will return a random point within the patrol radius
	FVector GetRandomPatrolPoint();
	//A Getter for the current 
	FVector GetCurrentPatrolPoint() const;

	//Getter for the current patrol center
	void SetCurrentPatrolCenter(const FVector &NewPatrolPoint) { CurrentPatrolCenter = NewPatrolPoint; }
	//Getter for the patrol zone origin
	void SetOriginPatrolZone(const FVector &NewCenter) { OriginPatrolZone = NewCenter; }

protected:
	virtual void BeginPlay() override;
};
