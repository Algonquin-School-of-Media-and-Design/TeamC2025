// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyCharacter.h"
#include "TormentedSoulCharacter.generated.h"

UCLASS()
class RELICRUNNERS_API ATormentedSoulCharacter : public AEnemyCharacter
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TEnumAsByte<EObjectTypeQuery>> AttackQuery;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsAttacking;

	UCapsuleComponent* AttackOverlapCollider;

public:
	ATormentedSoulCharacter();

	virtual void PrimaryAttack() override;

	virtual void Die(AController* EventInstigator) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


};
