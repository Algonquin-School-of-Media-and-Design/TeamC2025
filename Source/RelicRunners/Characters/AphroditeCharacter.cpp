// Fill out your copyright notice in the Description page of Project Settings.


#include "AphroditeCharacter.h"
#include "RelicRunners/AbilitySystem/AbilityPointCounter.h"
#include "RelicRunners/PlayerState/RelicRunnersPlayerState.h"


AAphroditeCharacter::AAphroditeCharacter()
{
}

void AAphroditeCharacter::BeginPlay()
{
    Super::BeginPlay();

    DamageCooldown = 5.0f;
    DefenceCooldown = 5.0f;
    UtilityCooldown = 5.0f;
    UltimateCooldown = 10.0f;

}

void AAphroditeCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    ARelicRunnersPlayerState* PS = GetPlayerState<ARelicRunnersPlayerState>();
    if (PS)
    {
        AbilitySystem = PS->GetAbilitySystemComponent();
        AbilitySystem->InitAbilityActorInfo(PS, this);
    }
}

void AAphroditeCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    ARelicRunnersPlayerState* PS = GetPlayerState<ARelicRunnersPlayerState>();
    if (PS)
    {
        AbilitySystem = PS->GetAbilitySystemComponent();
        AbilitySystem->InitAbilityActorInfo(PS, this);
    }
}

void AAphroditeCharacter::GiveDamageAbilities()
{
    Super::GiveDamageAbilities();

    if (AbilitySystem)
    {
        AbilitySystem->GiveAbility(FGameplayAbilitySpec(UBundleOfJoy::StaticClass(), 1, 0));
        AbilitySystem->TryActivateAbilityByClass(UBundleOfJoy::StaticClass());
        AbilityPointCounter->StartDamageCooldown(DamageCooldown);
    }
}
void AAphroditeCharacter::GiveDefenceAbilities()
{
	Super::GiveDefenceAbilities();

    if (AbilitySystem)
    {
        AbilityPointCounter->StartDefenceCooldown(DefenceCooldown);
    }
}

void AAphroditeCharacter::GiveUtilityAbilities()
{
	Super::GiveUtilityAbilities();

    if (AbilitySystem)
    {
        AbilityPointCounter->StartUtilityCooldown(UtilityCooldown);
    }
}

void AAphroditeCharacter::GiveUltimateAbilities()
{
	Super::GiveUltimateAbilities();

    if (AbilitySystem)
    {
        AbilityPointCounter->StartUltimateCooldown(UltimateCooldown);
    }
}


