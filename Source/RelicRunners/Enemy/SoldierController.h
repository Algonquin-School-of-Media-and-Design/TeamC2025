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
	UAIPerceptionComponent* AIPerceptionComp;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsInCombat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PatrolRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector CentarePatrolZone;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector CurrentPatrolPoint;
public:
	ASoldierController();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool GetIsInCombat() const;

	bool IsInPatrolRange();
	FVector GetRandomPatrolPoint();
	FVector GetCurrentPatrolPoint() const;

	void SetCurrentPatrolPoint(const FVector &NewPatrolPoint) { CurrentPatrolPoint = NewPatrolPoint; }
	void SetCentarePatrolZone(const FVector &NewCenter) { CentarePatrolZone = NewCenter; }

protected:
	virtual void BeginPlay() override;
};
