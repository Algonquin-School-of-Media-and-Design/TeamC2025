// Fill out your copyright notice in the Description page of Project Settings.


#include "AphroditeCharacter.h"
#include "RelicRunners/AbilitySystem/AbilityPointCounter.h"
#include "RelicRunners/PlayerState/RelicRunnersPlayerState.h"


AAphroditeCharacter::AAphroditeCharacter()
{

}

void AAphroditeCharacter::BeginPlay()
{
	Super::BeginPlay();

	DamageCooldown = 5.0f;
	DefenceCooldown = 5.0f;
	UtilityCooldown = 5.0f;
	UltimateCooldown = 10.0f;

}

void AAphroditeCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ARelicRunnersPlayerState* PS = GetPlayerState<ARelicRunnersPlayerState>();
	if (PS)
	{
		AbilitySystem = PS->GetAbilitySystemComponent();
		AbilitySystem->InitAbilityActorInfo(PS, this);
	}
}

void AAphroditeCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	ARelicRunnersPlayerState* PS = GetPlayerState<ARelicRunnersPlayerState>();
	if (PS)
	{
		AbilitySystem = PS->GetAbilitySystemComponent();
		AbilitySystem->InitAbilityActorInfo(PS, this);
	}
}
void AAphroditeCharacter::GiveDamageAbilities()
{
	Super::GiveDamageAbilities();

	if (!DamageClass)
	{
		DamageClass = ABundleOfJoy::StaticClass();
	}

	//For BundleOfJoy
	if (DamageClass && GetWorld())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 150.f + FVector(0, 0, 100.f);
		FRotator SpawnRotation = GetActorRotation();

		DamageAbilityInstance = GetWorld()->SpawnActor<AAbilityBase>(DamageClass, SpawnLocation, SpawnRotation, SpawnParams);

		if (DamageAbilityInstance)
		{
			DamageAbilityInstance->OwnerActor = this;
			DamageAbilityInstance->SetActorLocation(SpawnLocation);
			DamageAbilityInstance->ActivateAbility();
		}
	}
	AbilityPointCounter->StartDamageCooldown(DamageCooldown);
}

void AAphroditeCharacter::GiveDefenceAbilities()
{
	Super::GiveDefenceAbilities();

	AbilityPointCounter->StartDefenceCooldown(DefenceCooldown);
}

void AAphroditeCharacter::GiveUtilityAbilities()
{
	Super::GiveUtilityAbilities();

	AbilityPointCounter->StartUtilityCooldown(UtilityCooldown);
}

void AAphroditeCharacter::GiveUltimateAbilities()
{
	Super::GiveUltimateAbilities();

	AbilityPointCounter->StartUltimateCooldown(UltimateCooldown);
}


