// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "CooldownGameplayEffect.generated.h"

/**
 * 
 */
UCLASS()
class RELICRUNNERS_API UCooldownGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
	
public:
    UCooldownGameplayEffect();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cooldown")
	FGameplayTagContainer GrantedTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cooldown")
	FGameplayTagContainer OngoingTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cooldown")
	FGameplayTagContainer RemovedTags;
};
