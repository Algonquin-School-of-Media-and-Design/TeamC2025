// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "RelicRunnersGameState.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnObjectiveActionCompleted);

UENUM(BlueprintType)
enum class EObjectiveType : uint8
{
	None,
	Custom,
	CaptureTheFlag,
};

UCLASS()
class RELICRUNNERS_API ARelicRunnersGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	ARelicRunnersGameState();

	UFUNCTION(Server, Reliable)
	void Multicast_IncrementObjective();

	UFUNCTION(Server, Reliable)
	void Multicast_DecrementObjective();

	UFUNCTION(Server, Reliable)
	void Multicast_SetObjectiveType(EObjectiveType newType);

	bool InitializeTriggerState();

	UPROPERTY(ReplicatedUsing = OnRep_ObjectivesChange)
	int RemainingObjectives = 0;

	UFUNCTION()
	void OnRep_ObjectivesChange();

	UPROPERTY(BlueprintAssignable)
	FOnObjectiveActionCompleted OnObjectiveActionCompleted;

	UPROPERTY(Replicated)
	EObjectiveType ObjectiveType = EObjectiveType::None;
};
