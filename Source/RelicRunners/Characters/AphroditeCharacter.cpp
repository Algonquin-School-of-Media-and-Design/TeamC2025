// Fill out your copyright notice in the Description page of Project Settings.


#include "AphroditeCharacter.h"
#include "RelicRunners/PlayerState/RelicRunnersPlayerState.h"


AAphroditeCharacter::AAphroditeCharacter()
{

}

void AAphroditeCharacter::BeginPlay()
{
	Super::BeginPlay();

	
	APlayerState* PS = GetPlayerState();

	if (!PS) return;

	ARelicRunnersPlayerState* RPS = Cast<ARelicRunnersPlayerState>(PS);

	if (!RPS) return;


}

void AAphroditeCharacter::GiveInitialAbilities()
{
	Super::GiveInitialAbilities();

	AbilitySystem->GiveAbility(FGameplayAbilitySpec(ABundleOfJoy::StaticClass(), 1, 0));
}
