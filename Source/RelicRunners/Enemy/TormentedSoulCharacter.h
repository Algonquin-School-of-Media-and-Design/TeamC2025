// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyCharacter.h"
#include "TormentedSoulCharacter.generated.h"

UCLASS()
class RELICRUNNERS_API ATormentedSoulCharacter : public AEnemyCharacter
{
	friend class UTormentedSoulAnimNotifyState;
	GENERATED_BODY()
public:
	UPROPERTY(Category = "Enemy|Stats", EditAnywhere, BlueprintReadWrite)
	//The attack damage of the enemy
	float AttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//The collider object types to be check for with the attack collider
	TArray<TEnumAsByte<EObjectTypeQuery>> AttackQuery;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//If the actor is attacking
	bool IsAttacking;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//The collider that handles attacks
	UCapsuleComponent* AttackOverlapCollider;

public:
	//constructor
	ATormentedSoulCharacter();

	//method override from AActor handles take damage
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	//Method that is set for the OnOverlap delegate of a collider, that handles dealing damage to the player
	void BasicAttack(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
