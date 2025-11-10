// Fill out your copyright notice in the Description page of Project Settings.


#include "ArtemisCharacter.h"
#include "RelicRunners/AbilitySystem/AbilityPointCounter.h"
#include "RelicRunners/PlayerState/RelicRunnersPlayerState.h"

AArtemisCharacter::AArtemisCharacter()
{
}

void AArtemisCharacter::BeginPlay()
{
	Super::BeginPlay();

	DamageCooldown = 5.0f;
	DefenceCooldown = 5.0f;
	UtilityCooldown = 5.0f;
	UltimateCooldown = 10.0f;

}

void AArtemisCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);


	ARelicRunnersPlayerState* PS = GetPlayerState<ARelicRunnersPlayerState>();
	if (PS)
	{
		AbilitySystem = PS->GetAbilitySystemComponent();
		AbilitySystem->InitAbilityActorInfo(PS, this);
	}
}

void AArtemisCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	ARelicRunnersPlayerState* PS = GetPlayerState<ARelicRunnersPlayerState>();
	if (PS)
	{
		AbilitySystem = PS->GetAbilitySystemComponent();
		AbilitySystem->InitAbilityActorInfo(PS, this);
	}
}

void AArtemisCharacter::GiveDamageAbilities()
{
	Super::GiveDamageAbilities();

	AbilityPointCounter->StartDamageCooldown(DamageCooldown);
}

void AArtemisCharacter::GiveDefenceAbilities()
{
	Super::GiveDefenceAbilities();

	AbilityPointCounter->StartDefenceCooldown(DefenceCooldown);
}

void AArtemisCharacter::GiveUtilityAbilities()
{
	Super::GiveUtilityAbilities();

	AbilityPointCounter->StartUtilityCooldown(UtilityCooldown);
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
	AbilityPointCounter->StartUltimateCooldown(UltimateCooldown);
}

