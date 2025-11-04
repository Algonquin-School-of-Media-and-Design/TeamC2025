#include "NemesisCharacter.h"
#include "RelicRunners/PlayerState/RelicRunnersPlayerState.h"

ANemesisCharacter::ANemesisCharacter()
{
}

void ANemesisCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void ANemesisCharacter::GiveDamageAbilities()
{
	Super::GiveDamageAbilities();

}

void ANemesisCharacter::GiveDefenceAbilities()
{
	Super::GiveDefenceAbilities();
}

void ANemesisCharacter::GiveUtilityAbilities()
{
	Super::GiveUtilityAbilities();

	if (AbilitySystem)
	{
		AbilitySystem->GiveAbility(FGameplayAbilitySpec(AVengefulDance::StaticClass(), 1, 0));

		if (UtilityAbilityInstance)
		{
			UtilityAbilityInstance->ActivateAbility();
		}

	}
}

void ANemesisCharacter::GiveUltimateAbilities()
{
	Super::GiveUltimateAbilities();


}

