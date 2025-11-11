// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "RelicRunnersGameState.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnObjectiveActionCompleted);

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EObjectiveType : uint8
{
	None = 0 UMETA(Hidden),
	CaptureTheFlag = 1<<1,
	DefeatAllEnemies = 1<<2,
	DeliverPackage = 1<<3,
	DefendTheCrystal = 1<<4,
};
ENUM_CLASS_FLAGS(EObjectiveType);

inline bool operator== (int num, EObjectiveType type)
{
	return num == static_cast<uint8>(type);
}

inline bool operator|= (int a, EObjectiveType b)
{
	return a |= static_cast<uint8>(b);
}


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
	void Multicast_SetObjectiveType(int newType);

	bool InitializeTriggerState();

	UPROPERTY(ReplicatedUsing = OnRep_ObjectivesChange)
	int RemainingObjectives = 0;

	UFUNCTION()
	void OnRep_ObjectivesChange();

	UPROPERTY(BlueprintAssignable)
	FOnObjectiveActionCompleted OnObjectiveActionCompleted;

	UPROPERTY(Replicated, meta = (Bitmask, BitmaskEnum = EObjectiveType))
	int ObjectiveType = 0;
};
