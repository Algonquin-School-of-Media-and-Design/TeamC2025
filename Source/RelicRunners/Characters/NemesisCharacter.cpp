#include "NemesisCharacter.h"
#include "RelicRunners/PlayerState/RelicRunnersPlayerState.h"

ANemesisCharacter::ANemesisCharacter()
{
}

void ANemesisCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerState* PS = GetPlayerState();

	if (!PS) return;

	ARelicRunnersPlayerState* RPS = Cast<ARelicRunnersPlayerState>(PS);

	if (!RPS) return;

	if (RPS->SelectedClass == FName("Nemesis") && !bHasGivenInitialAbilities)
	{
		GiveInitialAbilities();
		bHasGivenInitialAbilities = true;
	}
}

void ANemesisCharacter::GiveInitialAbilities()
{
	Super::GiveInitialAbilities();

	if (AbilitySystem)
	{
		AbilitySystem->GiveAbility(FGameplayAbilitySpec(AVengefulDance::StaticClass(), 1, 0));
	}

}
