// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase.h"
#include "EarthquakeAbility.generated.h"

/**
 * 
 */
UCLASS()
class RELICRUNNERS_API AEarthquakeAbility : public AAbilityBase
{
    GENERATED_BODY()


public:
    AEarthquakeAbility();


    virtual void ActivateAbility() override;
    virtual void EndAbility() override;
    virtual bool CanActivate() const override;
    virtual FName GetAbilityName() const override { return FName("Earthquake"); }
    virtual void Tick(float DeltaTime) override;


protected:
    virtual void BeginPlay() override;


private:
    void ApplyDamageAndStun();


    UPROPERTY(EditDefaultsOnly, Category = "Earthquake")
    float Radius = 400.0f;


    UPROPERTY(EditDefaultsOnly, Category = "Earthquake")
    float TickRate = 1.0f;


    UPROPERTY(EditDefaultsOnly, Category = "Earthquake")
    float DamagePerTick = 10.0f;


    FTimerHandle TickTimerHandle;
    FTimerHandle DurationTimerHandle;
};