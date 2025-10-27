// BaseAbility.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilityInterface.h"
#include "AbilityBase.generated.h"

UCLASS(Abstract, Blueprintable)
class RELICRUNNERS_API AAbilityBase : public AActor, public IAbilityInterface
{
    GENERATED_BODY()

public:
    AAbilityBase();

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
    FName AbilityName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
    float Cooldown;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
    float Duration;

    UPROPERTY(BlueprintReadOnly, Category = "Ability")
    bool bIsOnCooldown;

    FTimerHandle CooldownTimer;

public:
    virtual void ActivateAbility() override;
    virtual void EndAbility() override;
    virtual bool CanActivate() const override;
    virtual FName GetAbilityName() const override { return AbilityName; }

protected:
    void StartCooldown();
    void ResetCooldown();
};
