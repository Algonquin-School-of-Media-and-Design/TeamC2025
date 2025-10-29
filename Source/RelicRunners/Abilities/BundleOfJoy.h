// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RelicRunners/AbilitySystem/AbilityBase.h"
#include "BundleOfJoy.generated.h"

UCLASS()
class RELICRUNNERS_API ABundleOfJoy : public AAbilityBase
{
	GENERATED_BODY()
	
public:	

	ABundleOfJoy();
	virtual void ActivateAbility() override;
	virtual void EndAbility() override;
	virtual bool CanActivate() const override;
	virtual FName GetAbilityName() const override { return FName("Bundle Of Joy"); }

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void Explode();
	void Attract();

};
