#pragma once

#include "CoreMinimal.h"
#include "RelicRunners/AbilitySystem/AbilityBase.h"
#include "VengefulDance.generated.h"

UCLASS()
class RELICRUNNERS_API AVengefulDance : public AAbilityBase 
{
    GENERATED_BODY()

public:
    AVengefulDance();

    virtual void ActivateAbility() override;
    virtual void EndAbility() override;
	virtual bool CanActivate() const override;
	virtual FName GetAbilityName() const override { return FName("Vengeful Dance"); }

    // Called every frame
    virtual void Tick(float DeltaTime) override;

protected:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VengefulDance")
    float DamageInterval;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VengefulDance")
    float KnockbackStrength;


    virtual void BeginPlay() override;
private:

    FTimerHandle DamageTickTimer;

    FTimerHandle EndTimer;

    void ApplyRingDamage();
};
