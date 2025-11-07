#include "NemesisCharacter.h"
#include "RelicRunners/PlayerState/RelicRunnersPlayerState.h"

ANemesisCharacter::ANemesisCharacter()
{
}

void ANemesisCharacter::BeginPlay()
{
	Super::BeginPlay();

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

