// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityBase.h"

// Sets default values
AAbilityBase::AAbilityBase()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AAbilityBase::ActivateAbility()
{

}

void AAbilityBase::EndAbility()
{

}

bool AAbilityBase::CanActivate() const
{
	return false;
}

void AAbilityBase::StartCooldown()
{

}

void AAbilityBase::ResetCooldown()
{
}
