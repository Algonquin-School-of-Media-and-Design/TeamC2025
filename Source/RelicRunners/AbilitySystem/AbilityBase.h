#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"  
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

    // Ability Tags
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability|Tags")
    FGameplayTagContainer BlockAbilitiesWithTags;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability|Tags")
    FGameplayTagContainer CancelAbilitiesWithTags;

};

