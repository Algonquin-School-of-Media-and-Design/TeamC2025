#include "NemesisCharacter.h"
#include "RelicRunners/AbilitySystem/AbilityPointCounter.h"
#include "RelicRunners/PlayerState/RelicRunnersPlayerState.h"

ANemesisCharacter::ANemesisCharacter()
{
}

void ANemesisCharacter::BeginPlay()
{
	Super::BeginPlay();

    DamageCooldown = 5.0f;
    DefenceCooldown = 5.0f;
    UtilityCooldown = 5.0f;
    UltimateCooldown = 10.0f;

}

void ANemesisCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    
    ARelicRunnersPlayerState* PS = GetPlayerState<ARelicRunnersPlayerState>();
    if (PS)
    {
        AbilitySystem = PS->GetAbilitySystemComponent(); 
        AbilitySystem->InitAbilityActorInfo(PS, this);   
    }
}

void ANemesisCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    ARelicRunnersPlayerState* PS = GetPlayerState<ARelicRunnersPlayerState>();
    if (PS)
    {
        AbilitySystem = PS->GetAbilitySystemComponent();
        AbilitySystem->InitAbilityActorInfo(PS, this);
    }
}

void ANemesisCharacter::GiveDamageAbilities()
{
	Super::GiveDamageAbilities();

	if (AbilitySystem)
	{
        AbilityPointCounter->StartDamageCooldown(DamageCooldown);
	}
}

void ANemesisCharacter::GiveDefenceAbilities()
{
	Super::GiveDefenceAbilities();

    if (AbilitySystem)
    {
        AbilitySystem->GiveAbility(FGameplayAbilitySpec(UImpunityAbility::StaticClass(), 1, 0));
        AbilitySystem->TryActivateAbilityByClass(UImpunityAbility::StaticClass());
        AbilityPointCounter->StartDefenceCooldown(DefenceCooldown);
    }
}

void ANemesisCharacter::GiveUtilityAbilities()
{
	Super::GiveUtilityAbilities();

    if (AbilitySystem)
    {
        AbilitySystem->GiveAbility(FGameplayAbilitySpec(UVengefulDance::StaticClass(), 1, 0));
        AbilitySystem->TryActivateAbilityByClass(UVengefulDance::StaticClass());
        AbilityPointCounter->StartUtilityCooldown(UtilityCooldown);
    }
}

void ANemesisCharacter::GiveUltimateAbilities()
{
	Super::GiveUltimateAbilities();

	if (AbilitySystem)
	{
        AbilityPointCounter->StartUltimateCooldown(UltimateCooldown);
	}
    
}

