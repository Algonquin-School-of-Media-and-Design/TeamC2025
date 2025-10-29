// Fill out your copyright notice in the Description page of Project Settings.


#include "BundleOfJoy.h"

// Sets default values
ABundleOfJoy::ABundleOfJoy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ABundleOfJoy::BeginPlay()
{
	Super::BeginPlay();

}

void ABundleOfJoy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABundleOfJoy::ActivateAbility()
{
}

void ABundleOfJoy::EndAbility()
{
}

bool ABundleOfJoy::CanActivate() const
{
	return false;
}

void ABundleOfJoy::Explode()
{
}

void ABundleOfJoy::Attract()
{
}
