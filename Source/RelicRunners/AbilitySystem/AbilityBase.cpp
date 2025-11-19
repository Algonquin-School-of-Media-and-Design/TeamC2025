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

    CooldownGameplayEffectClass = GenericCooldownEffect;
}







