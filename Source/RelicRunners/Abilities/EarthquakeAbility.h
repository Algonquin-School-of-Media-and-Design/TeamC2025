// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RelicRunners/AbilitySystem/AbilityBase.h"
#include "EarthquakeAbility.generated.h"

/**
 * 
 */
UCLASS()
class RELICRUNNERS_API UEarthquakeAbility : public UAbilityBase
{
    GENERATED_BODY()

public:
    UEarthquakeAbility();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
    virtual FName GetAbilityName() const override { return FName("Earthquake"); }

protected:

    void OnTick();
    void EndAbilityTimerCallback();

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Earthquake")
    float Radius = 400.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Earthquake")
    float TickRate = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Earthquake")
    float DamagePerTick = 10.0f;

    FGameplayAbilityActivationInfo CachedActivationInfo;
private:
    FTimerHandle TickTimerHandle;
    FTimerHandle DurationTimerHandle;
    bool bIsOnCooldown = false;

    void ApplyDamageAndStun();
};