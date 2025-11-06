#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilityInterface.h"
#include "AbilityBase.generated.h"

UENUM(BlueprintType)
enum class EAbilityCategory : uint8
{
    Offensive,
    Defensive,
    Utility,
    Ultimate
};

UENUM(BlueprintType)
enum class EAbilityTargetType : uint8
{
    None,
    Self,
    SingleEnemy,
    Area,
    Cone
};

UCLASS(Abstract, Blueprintable)
class RELICRUNNERS_API AAbilityBase : public AActor, public IAbilityInterface
{
    GENERATED_BODY()

public:
    AAbilityBase();

    virtual void ActivateAbility() override;
    virtual void EndAbility() override;
    virtual bool CanActivate() const override;
    virtual FName GetAbilityName() const override;
    
    UFUNCTION(BlueprintCallable, Category = "Ability|Timing")
    float GetCooldown() const { return Cooldown; }

    UFUNCTION(BlueprintCallable, Category = "Ability|Owner")
    void SetAbilityOwner(AActor* InOwner) { OwnerActor = InOwner; }

protected:
    UPROPERTY(BlueprintReadWrite, Category = "Ability|State")
    bool bIsOnCooldown;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|State")
    bool bCanBeInterrupted;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Timing")
    float Cooldown;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Timing")
    float Duration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Combat")
    float DamageAmount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Combat")
    float AreaRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Combat")
    float ConeAngle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Info")
    FName AbilityName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Info")
    EAbilityCategory AbilityCategory;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Info")
    EAbilityTargetType TargetType;

    UPROPERTY(BlueprintReadWrite, Category = "Ability|Owner")
    AActor* OwnerActor;
};
