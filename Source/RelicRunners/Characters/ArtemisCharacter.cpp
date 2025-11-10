// Fill out your copyright notice in the Description page of Project Settings.


#include "ArtemisCharacter.h"
#include "RelicRunners/PlayerState/RelicRunnersPlayerState.h"

AArtemisCharacter::AArtemisCharacter()
{
}

void AArtemisCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void AArtemisCharacter::GiveDamageAbilities()
{
	Super::GiveDamageAbilities();
}

void AArtemisCharacter::GiveDefenceAbilities()
{
	Super::GiveDefenceAbilities();
}

void AArtemisCharacter::GiveUtilityAbilities()
{
	Super::GiveUtilityAbilities();
}

void AArtemisCharacter::GiveUltimateAbilities()
{
	Super::GiveUltimateAbilities();

	if (!UltimateClass)
	{
		UltimateClass = AMoonbeam::StaticClass();
	}

	if (UltimateClass)
	{
		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.Instigator = this;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		UltimateAbilityInstance = GetWorld()->SpawnActor<AAbilityBase>(
			UltimateClass,
			GetActorLocation(),
			GetActorRotation(),
			Params
		);

		if (UltimateAbilityInstance)
		{
			UltimateAbilityInstance->SetAbilityOwner(this);
		}
	}

	if (UltimateAbilityInstance)
	{
		if (UltimateAbilityInstance->CanActivate())
		{
			UltimateAbilityInstance->ActivateAbility();
		}
	}

}

