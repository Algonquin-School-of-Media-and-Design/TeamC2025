// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase.h"
#include "WarBannerAbility.generated.h"

/**
 * 
 */
UCLASS()
class RELICRUNNERS_API AWarBannerAbility : public AAbilityBase
{
	GENERATED_BODY()
	
public:
	AWarBannerAbility();

	void Initialize(AActor* newOwner);

	virtual void ActivateAbility() override;
	virtual void EndAbility() override;
	virtual bool CanActivate() const override;
	virtual FName GetAbilityName() const override;

	void CancelAbility();

	void SpawnBanner();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* BannerSilhouetteMesh;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWarBanner> WarBannerTemplate;

	class AWarBanner* WarBanner;
};
