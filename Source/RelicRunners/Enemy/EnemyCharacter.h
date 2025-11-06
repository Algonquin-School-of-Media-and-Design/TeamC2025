// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

class UItemObject;
class UWidgetComponent;

USTRUCT(BlueprintType)
struct FAttackStartInfo
{
	GENERATED_BODY()
	FVector AttackOriginPoint;
};

USTRUCT(BlueprintType)
struct FAttackEndInfo
{
	GENERATED_BODY()
	FVector AttackEndPoint;
};

UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	LIGHT = 0		UMETA(DisplayName = "light"),
	MEDIUM = 1		UMETA(DisplayName = "medium"),
	BOSS = 2        UMETA(DisplayName = "boss"),
	UNKNOWN = 3		UMETA(DisplayName = "unknown")
};

UCLASS(Blueprintable, Abstract)
class RELICRUNNERS_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UItemObject*> ItemLootPool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	float CurrentHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	float RemainingStunTime = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int Level;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	EEnemyType TypeOfEnemy;
	UPROPERTY(VisibleAnywhere, Replicated)
	UWidgetComponent* EnemyHUDWorld;

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UFUNCTION(BlueprintCallable)
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	virtual void Die(AController* EventInstigator);

	UFUNCTION(BlueprintCallable)
	virtual bool IsDead() { return CurrentHealth <= 0; }

	UFUNCTION(BlueprintCallable)
	void ReduceStunTime(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	float GetCurrentHealth() { return CurrentHealth; } const
	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() { return MaxHealth; } const
	UFUNCTION(BlueprintCallable)
	float GetStunTime() { return RemainingStunTime; } const
	UFUNCTION(BlueprintCallable)
	int GetEnemyLevel() { return Level; } const
	UFUNCTION(BlueprintCallable)
	EEnemyType GetEnemyType() { return TypeOfEnemy; } const

	UFUNCTION(BlueprintCallable)
	void SetCurrentHealth(const float& newHealth) { CurrentHealth = newHealth; }
	UFUNCTION(BlueprintCallable)
	void SetMaxHealth(const float& newHealth) { MaxHealth = newHealth; }
	UFUNCTION(BlueprintCallable)
	void SetStunTime(const float& newStunTime) { RemainingStunTime = newStunTime; }
	UFUNCTION(BlueprintCallable)
	void SetEnemyLevel(const int& newLevel) { Level = newLevel; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	virtual void SpawnItem();

	UFUNCTION(BlueprintCallable)
	virtual float CalculateXP() const;

	void UpdateEnemyHUDWorldFacing();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
