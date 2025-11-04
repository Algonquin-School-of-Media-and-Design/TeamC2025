// Fill out your copyright notice in the Description page of Project Settings.


#include "AresCharacter.h"
#include "RelicRunners/PlayerState/RelicRunnersPlayerState.h"

AAresCharacter::AAresCharacter()
{
}

void AAresCharacter::BeginPlay()
{
	Super::BeginPlay();


	APlayerState* PS = GetPlayerState();

	if (!PS) return;

	ARelicRunnersPlayerState* RPS = Cast<ARelicRunnersPlayerState>(PS);

	if (!RPS) return;
}

void AAresCharacter::GiveInitialAbilities()
{
	Super::GiveInitialAbilities();

	//AbilitySystem->GiveAbility(FGameplayAbilitySpec(AWarBannerAbility::StaticClass(), 1, 0));

}


