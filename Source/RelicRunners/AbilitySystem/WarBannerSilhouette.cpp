// Fill out your copyright notice in the Description page of Project Settings.


#include "WarBannerSilhouette.h"

// Sets default values
AWarBannerSilhouette::AWarBannerSilhouette()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AWarBannerSilhouette::SetVisibility(bool newVisibility)
{
	BannerSilhouetteMesh->SetVisibility(newVisibility);
}
