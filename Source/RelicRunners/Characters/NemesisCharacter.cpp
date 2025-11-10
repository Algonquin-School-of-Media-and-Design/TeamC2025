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

    AbilityPointCounter->StartDamageCooldown(DamageCooldown);
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

    AbilityPointCounter->StartDefenceCooldown(DefenceCooldown);
}

void ANemesisCharacter::GiveUtilityAbilities()
{
	Super::GiveUtilityAbilities();

    UtilityClass = AVengefulDance::StaticClass();
    
    if (AbilitySystem)
    {
        AbilitySystem->GiveAbility(FGameplayAbilitySpec(AVengefulDance::StaticClass(), 1, 0));

        if (UtilityClass)
        {
            UtilityAbilityInstance = GetWorld()->SpawnActor<AAbilityBase>(UtilityClass);
            if (UtilityAbilityInstance)
            {
                UtilityAbilityInstance->OwnerActor = this;
                UtilityAbilityInstance->ActivateAbility();
            }
        }
	}

    AbilityPointCounter->StartUtilityCooldown(UtilityCooldown);
}

void ANemesisCharacter::GiveUltimateAbilities()
{
	Super::GiveUltimateAbilities();

    AbilityPointCounter->StartUltimateCooldown(UltimateCooldown);
}

