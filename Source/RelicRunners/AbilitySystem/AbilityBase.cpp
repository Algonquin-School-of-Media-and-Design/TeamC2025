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

    AbilityName = FName("BaseAbility");

}

void UAbilityBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (!CooldownTag.IsValid())
    {

        CooldownTag = FGameplayTag::RequestGameplayTag(FName(*FString::Printf(TEXT("Cooldown.%s"), *AbilityName.ToString())));
        ActivationBlockedTags.AddTag(CooldownTag);
    }

    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    if (CooldownDuration > 0.f && CooldownGameplayEffectClass)
    {
        ApplyCooldown(Handle, ActorInfo, ActivationInfo);
    }
}

void UAbilityBase::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

    // put nothing here
}

void UAbilityBase::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
    if (!CooldownGameplayEffectClass) return;

    UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
    if (!ASC) return;

    ASC->ApplyGameplayEffectToSelf(CooldownGameplayEffectClass.GetDefaultObject(),1.f,ASC->MakeEffectContext());
}


