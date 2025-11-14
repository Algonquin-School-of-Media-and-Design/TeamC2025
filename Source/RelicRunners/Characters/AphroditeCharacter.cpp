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

	// set cooldown durations
    DamageCooldown = 5.0f;
    DefenceCooldown = 5.0f;
    UtilityCooldown = 5.0f;
    UltimateCooldown = 10.0f;

}

// Called when player takes control of this character
void AAphroditeCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    // Get PlayerState and set up ability system
    ARelicRunnersPlayerState* PS = GetPlayerState<ARelicRunnersPlayerState>();
    if (PS)
    {
        AbilitySystem = PS->GetAbilitySystemComponent();
        AbilitySystem->InitAbilityActorInfo(PS, this);
    }
}

// Called when PlayerState is replicated on clients
void AAphroditeCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    // Reinitialize ability system
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
		// Give Bundle of Joy ability 
        AbilitySystem->GiveAbility(FGameplayAbilitySpec(UBundleOfJoy::StaticClass(), 1, 0));
		// Activate Bundle of Joy ability
        AbilitySystem->TryActivateAbilityByClass(UBundleOfJoy::StaticClass());
		// start ability cooldown
        AbilityPointCounter->StartDamageCooldown(DamageCooldown);
    }
}
void AAphroditeCharacter::GiveDefenceAbilities()
{
	Super::GiveDefenceAbilities();

    if (AbilitySystem)
    {
		// start ability cooldown
        AbilityPointCounter->StartDefenceCooldown(DefenceCooldown);
    }
}

void AAphroditeCharacter::GiveUtilityAbilities()
{
	Super::GiveUtilityAbilities();

    if (AbilitySystem)
    {
		// start ability cooldown
        AbilityPointCounter->StartUtilityCooldown(UtilityCooldown);
    }
}

void AAphroditeCharacter::GiveUltimateAbilities()
{
	Super::GiveUltimateAbilities();

    if (AbilitySystem)
    {
		// start ability cooldown
        AbilityPointCounter->StartUltimateCooldown(UltimateCooldown);
    }
}


