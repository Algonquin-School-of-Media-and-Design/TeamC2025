// Fill out your copyright notice in the Description page of Project Settings.


#include "ArtemisCharacter.h"
#include "RelicRunners/PlayerState/RelicRunnersPlayerState.h"

AArtemisCharacter::AArtemisCharacter()
{
}

void AArtemisCharacter::BeginPlay()
{
	Super::BeginPlay();


	APlayerState* PS = GetPlayerState();

	if (!PS) return;

	ARelicRunnersPlayerState* RPS = Cast<ARelicRunnersPlayerState>(PS);

	if (!RPS) return;
}

void AArtemisCharacter::GiveInitialAbilities()
{
    Super::GiveInitialAbilities();

    //AbilitySystem->GiveAbility(FGameplayAbilitySpec(::StaticClass(), 1, 0));
}
