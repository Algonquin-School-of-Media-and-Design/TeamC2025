// Fill out your copyright notice in the Description page of Project Settings.


#include "AresCharacter.h"
#include "RelicRunners/PlayerState/RelicRunnersPlayerState.h"

AAresCharacter::AAresCharacter()
{
}

void AAresCharacter::BeginPlay()
{
	Super::BeginPlay();

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


}

void AAresCharacter::GiveDefenceAbilities()
{
	Super::GiveDefenceAbilities();
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
}

void AAresCharacter::GiveUltimateAbilities()
{
	Super::GiveUltimateAbilities();
}




