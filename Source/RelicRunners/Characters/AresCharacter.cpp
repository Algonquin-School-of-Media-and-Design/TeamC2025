// Fill out your copyright notice in the Description page of Project Settings.


#include "AresCharacter.h"
#include "RelicRunners/AbilitySystem/AbilityPointCounter.h"
#include "RelicRunners/PlayerState/RelicRunnersPlayerState.h"

AAresCharacter::AAresCharacter()
{
}

void AAresCharacter::BeginPlay()
{
	Super::BeginPlay();

	DamageCooldown = 5.0f;
	DefenceCooldown = 5.0f;
	UtilityCooldown = 5.0f;
	UltimateCooldown = 10.0f;

}

void AAresCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);


	ARelicRunnersPlayerState* PS = GetPlayerState<ARelicRunnersPlayerState>();
	if (PS)
	{
		AbilitySystem = PS->GetAbilitySystemComponent();
		AbilitySystem->InitAbilityActorInfo(PS, this);
	}
}

void AAresCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	ARelicRunnersPlayerState* PS = GetPlayerState<ARelicRunnersPlayerState>();
	if (PS)
	{
		AbilitySystem = PS->GetAbilitySystemComponent();
		AbilitySystem->InitAbilityActorInfo(PS, this);
	}
}

void AAresCharacter::GiveDamageAbilities()
{
	Super::GiveDamageAbilities();

	if (!DamageClass)
	{
		DamageClass = AEarthquakeAbility::StaticClass();
	}

	if (DamageClass)
	{
		DamageAbilityInstance = GetWorld()->SpawnActor<AAbilityBase>(DamageClass);
		if (DamageAbilityInstance)
		{
			DamageAbilityInstance->OwnerActor = this;
		}
	}

	if (DamageAbilityInstance)
	{
		DamageAbilityInstance->ActivateAbility();
	}

	AbilityPointCounter->StartDamageCooldown(DamageCooldown);
}

void AAresCharacter::GiveDefenceAbilities()
{
	Super::GiveDefenceAbilities();

	AbilityPointCounter->StartDefenceCooldown(DefenceCooldown);
}

void AAresCharacter::GiveUtilityAbilities()
{
	Super::GiveUtilityAbilities();

	//press ability key
	//IsWarBannerActive = !IsWarBannerActive;

	//if (WarBannerAbility == nullptr)
	//	return;

	//if (IsWarBannerActive)
	//{
	//	WarBannerAbility->ActivateAbility();
	//}
	//else
	//{
	//	WarBannerAbility->CancelAbility();
	//}

	//// aiming with the mouse
	//if (IsWarBannerActive)
	//{
	//	FHitResult HitResult;
	//	FCollisionQueryParams TraceParams(FName(TEXT("LineTrace")), true, this);

	//	bool bHit = GetWorld()->LineTraceSingleByChannel(
	//		HitResult,
	//		PlayerLocation,
	//		PlayerLocation + (PlayerForward * 1000.0f),
	//		ECC_Visibility,
	//		TraceParams);

	//	FVector targetPosition = bHit ? HitResult.Location : PlayerLocation + (PlayerForward * 1000.0f);

	//	DrawDebugLine(GetWorld(), PlayerLocation, targetPosition, FColor::Blue);
	//	WarBannerAbility->SetActorLocation(targetPosition);
	//	WarBannerAbility->SetActorRotation(FRotator(0.0f, GetControlRotation().Yaw, 0.0f));

	//	//on let click
	//	if (IsWarBannerActive && WarBannerAbility != nullptr)
	//	{
	//		WarBannerAbility->Server_SpawnBanner();
	//	}
	//}

	//spawning the actor
	//if (WarBannerAbilityTemplate != nullptr)
	//{
	//	WarBannerAbility = GetWorld()->SpawnActor<AWarBannerAbility>(WarBannerAbilityTemplate, FVector::ZeroVector, FRotator::ZeroRotator);
	//	WarBannerAbility->Server_Initialize(this);
	//}

	AbilityPointCounter->StartUtilityCooldown(UtilityCooldown);
}

void AAresCharacter::GiveUltimateAbilities()
{
	Super::GiveUltimateAbilities();

	AbilityPointCounter->StartUltimateCooldown(UltimateCooldown);
}




