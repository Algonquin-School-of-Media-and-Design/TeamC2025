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

    if (!DefenceClass)
    {
        DefenceClass = AImpunityAbility::StaticClass();
    }

    if (DefenceClass)
    {
        DefenceAbilityInstance = GetWorld()->SpawnActor<AAbilityBase>(DefenceClass);
        if (DefenceAbilityInstance)
        {
            DefenceAbilityInstance->OwnerActor = this;
        }
    }

    if (DefenceAbilityInstance)
    {
        DefenceAbilityInstance->ActivateAbility();
    }
}

void ANemesisCharacter::GiveUtilityAbilities()
{
	Super::GiveUtilityAbilities();

    UtilityClass = AVengefulDance::StaticClass();

    if (UtilityClass)
    {
        UtilityAbilityInstance = GetWorld()->SpawnActor<AAbilityBase>(UtilityClass);
        if (UtilityAbilityInstance)
        {
            UtilityAbilityInstance->OwnerActor = this;
        }
    }

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

