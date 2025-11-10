// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RelicRunners/AbilitySystem/AbilityBase.h"
#include "ImpunityAbility.generated.h"

/**
 * 
 */
UCLASS()
class RELICRUNNERS_API AImpunityAbility : public AAbilityBase
{
    GENERATED_BODY()

public:
    AImpunityAbility();

    virtual void ActivateAbility() override;
    virtual void EndAbility() override;
    virtual bool CanActivate() const override;
    virtual FName GetAbilityName() const override { return FName("Impunity"); }
    float GetDamageReductionMultiplier() const { return bIsActive ? 0.5f : 1.0f; } // 50% reduction when active 

protected:
    virtual void BeginPlay() override;

private:
    void ResetSpeed();

    FTimerHandle SpeedResetTimer;

    float OriginalSpeed;
};