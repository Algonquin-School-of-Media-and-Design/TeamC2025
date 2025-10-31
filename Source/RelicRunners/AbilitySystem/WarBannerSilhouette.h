// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WarBannerSilhouette.generated.h"

UCLASS()
class RELICRUNNERS_API AWarBannerSilhouette : public AActor
{
	GENERATED_BODY()
	
public:	
	AWarBannerSilhouette();

	void SetVisibility(bool newVisibility);

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* BannerSilhouetteMesh;

};
