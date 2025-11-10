#pragma once

#include "CoreMinimal.h"
#include "RelicRunners/AbilitySystem/AbilityBase.h"
#include "Moonbeam.generated.h"

UCLASS()
class RELICRUNNERS_API AMoonbeam : public AAbilityBase
{
    GENERATED_BODY()

public:
    AMoonbeam();

    virtual void ActivateAbility() override;
    virtual void EndAbility() override;
    virtual bool CanActivate() const override;

protected:
    // How far the beam goes
    UPROPERTY(EditDefaultsOnly, Category = "Moonbeam")
    float BeamRange;

    // Visual thickness for debug 
    UPROPERTY(EditDefaultsOnly, Category = "Moonbeam")
    float BeamRadius;

    // How long the debug beam stays visible
    UPROPERTY(EditDefaultsOnly, Category = "Moonbeam")
    float BeamDuration;

    // Internal cooldown timer for this ability
    FTimerHandle CooldownTimerHandle;

    void ResetCooldown();
    void SpawnBeamEffect(const FVector& Start, const FVector& End);
};
