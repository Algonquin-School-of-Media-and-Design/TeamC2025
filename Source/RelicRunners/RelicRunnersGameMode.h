/************************************************************************************
 * Author:   Tristan Anglin
 * Year:     2025
 *
 * Notice:
 *   This source code is the intellectual property of Tristan Anglin.
 *   It is provided solely for use within:
 *     - The Relic Runners personal project
 *     - The Almost A Dozen project
 *
 *   Any use, distribution, or modification outside of these projects
 *   is strictly prohibited without explicit written permission.
 *
 *   © 2025 Tristan Anglin. All rights reserved.
 ************************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RelicRunnersGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnObjectiveActionCompleted);

UCLASS(minimalapi)
class ARelicRunnersGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARelicRunnersGameMode();
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void PostSeamlessTravel() override;
	UPROPERTY(EditDefaultsOnly, Category = "Preview")
	TSubclassOf<class APlayerPreview> PlayerPreviewClass;


	//TODO: Maybe make an Objectives Handler Object
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_InitializeObjectives(int maxObjectives);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_DecrementObjective();

	UPROPERTY(Replicated)
	int MaxObjectives = 0;

	UPROPERTY(Replicated)
	int RemainingObjectives = 0;

	UPROPERTY(BlueprintAssignable)
	FOnObjectiveActionCompleted OnObjectiveActionCompleted;
};



