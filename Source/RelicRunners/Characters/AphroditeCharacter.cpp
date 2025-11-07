// Fill out your copyright notice in the Description page of Project Settings.


#include "AphroditeCharacter.h"
#include "RelicRunners/PlayerState/RelicRunnersPlayerState.h"


AAphroditeCharacter::AAphroditeCharacter()
{

}

void AAphroditeCharacter::BeginPlay()
{
	Super::BeginPlay();


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

}

void AAphroditeCharacter::GiveDefenceAbilities()
{
	Super::GiveDefenceAbilities();
}

void AAphroditeCharacter::GiveUtilityAbilities()
{
	Super::GiveUtilityAbilities();
}

void AAphroditeCharacter::GiveUltimateAbilities()
{
	Super::GiveUltimateAbilities();
}


