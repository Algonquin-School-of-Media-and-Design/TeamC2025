#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"  
#include "CooldownGameplayEffect.h"
#include "AbilityBase.generated.h"

UENUM(BlueprintType)
enum class EAbilityCategory : uint8
{
    Offensive,
    Defensive,
    Utility,
    Ultimate, 
    None
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
class RELICRUNNERS_API UAbilityBase : public UGameplayAbility
{
    GENERATED_BODY()

public:
    UAbilityBase();

    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData
    ) override;

    virtual void EndAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        bool bReplicateEndAbility,
        bool bWasCancelled
    ) override;

    // GAS Cooldown
    virtual void ApplyCooldown(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo
    ) const override;

    virtual const FGameplayTagContainer* GetCooldownTags() const override;
    virtual UGameplayEffect* GetCooldownGameplayEffect() const override;
    virtual FName GetAbilityName() const { return FName(TEXT("AbilityBase")); }

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Config")
    float Duration = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Combat")
    float DamageAmount = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Combat")
    float AreaRadius = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Combat")
    float ConeAngle = 0.f;

    //Ability Info
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Info")
    FName AbilityName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Info")
    EAbilityCategory AbilityCategory;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Info")
    EAbilityTargetType TargetType;


    // GAS Cooldown

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Cooldown")
    float CooldownDuration = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability|Cooldown")
    FGameplayTag CooldownTag;

    UPROPERTY()
    FGameplayTagContainer CooldownTagContainer;

    UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
    TSubclassOf<UGameplayEffect> GenericCooldownEffect;

};

