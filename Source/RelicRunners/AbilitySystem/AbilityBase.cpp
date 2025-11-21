#include "AbilityBase.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"

UAbilityBase::UAbilityBase()
{
    // Set ability to be unique per actor
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    //CooldownGameplayEffectClass = GenericCooldownEffect;
}

void UAbilityBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UAbilityBase::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

//void UAbilityBase::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
//{
//    Super::ApplyCooldown(Handle, ActorInfo, ActivationInfo);
//
//    if (!GenericCooldownEffect)
//    {
//        UE_LOG(LogTemp, Error, TEXT("[%s] GenericCooldownEffect is NULL"), *AbilityName.ToString());
//        return;
//    }
//
//    FGameplayEffectSpecHandle Spec = MakeOutgoingGameplayEffectSpec(GenericCooldownEffect, 1);
//    Spec.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.CooldownDuration"), CooldownDuration);
//
//    ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, Spec);
//
//    UE_LOG(LogTemp, Warning, TEXT("[%s] Cooldown applied!"), *AbilityName.ToString());
//
//   
//}

//const FGameplayTagContainer* UAbilityBase::GetCooldownTags() const
//{
//    return &CooldownTagContainer;
//}
//
//UGameplayEffect* UAbilityBase::GetCooldownGameplayEffect() const
//{
//    return GenericCooldownEffect ? GenericCooldownEffect->GetDefaultObject<UGameplayEffect>() : nullptr;
//}







