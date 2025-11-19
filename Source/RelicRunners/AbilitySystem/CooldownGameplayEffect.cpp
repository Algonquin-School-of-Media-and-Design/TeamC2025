// Fill out your copyright notice in the Description page of Project Settings.

#include "CooldownGameplayEffect.h"
#include "GameplayTagContainer.h"

UCooldownGameplayEffect::UCooldownGameplayEffect()
{
    DurationPolicy = EGameplayEffectDurationType::HasDuration;

    FScalableFloat DefaultDuration(5.f);
    DurationMagnitude = FGameplayEffectModifierMagnitude(DefaultDuration);

}