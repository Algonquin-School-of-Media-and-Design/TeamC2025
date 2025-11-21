// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

class UWidgetComponent;

UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	LIGHT = 0		UMETA(DisplayName = "light"),
	MEDIUM = 1		UMETA(DisplayName = "medium"),
	BOSS = 2        UMETA(DisplayName = "boss"),
	UNKNOWN = 3		UMETA(DisplayName = "unknown")
};

class UAnimMontage;

UCLASS(Blueprintable, Abstract)
class RELICRUNNERS_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UItemCard*> ItemLootPool;

	UPROPERTY(DisplayName = "Name", Category = "Enemy", EditAnywhere, BlueprintReadWrite, Replicated, meta = (ExposeOnSpawn = true, NoResetToDefault, DisplayPriority = 0))
	FName EnemyName = "Enemy";
	UPROPERTY(Category = "Enemy|Stats", EditAnywhere, BlueprintReadWrite, Replicated, BlueprintSetter = SetCurrentHealth, meta = (ClampMin = 0, UIMin = 0, Delta = 10))
	float CurrentHealth;
	UPROPERTY(Category = "Enemy|Stats", EditAnywhere, BlueprintReadWrite, Replicated, BlueprintSetter = SetMaxHealth, meta = (ClampMin = 0, UIMin = 0, UIMax = 10000, Delta = 10))
	float MaxHealth;
	UPROPERTY(Category = "Enemy|Stats", EditAnywhere, BlueprintReadWrite, Replicated, meta = (Delta = 0.1))
	float RemainingStunTime = 0;
	UPROPERTY(Category = "Enemy|Stats", EditAnywhere, BlueprintReadWrite, Replicated, meta = (Delta = 1))
	int Level = 1;
	UPROPERTY(Category = "Enemy|Stats", EditAnywhere, Replicated, BlueprintReadWrite, meta = (ClampMin = 0, UIMin = 0, Delta = 10))
	//A Stand in for mana, energy, or stamina
	int EnemyResource = 100;

	UPROPERTY(Category = "Enemy|Stats", EditAnywhere, Replicated, BlueprintReadWrite, meta = (ClampMin = 0, UIMin = 0, UIMax = 10000, Delta = 10))
	//A Stand in for mana, energy, or stamina
	int EnemyMaxResource = 100;

	UPROPERTY(Category = "Enemy", EditAnywhere, Replicated, BlueprintReadWrite, meta = (ClampMin = 0, ClampMax = 1, UIMin = 0, UIMax = 1, Delta = 0.05, ToolTip = "The Chance the enemy has to drop an item on death from 0.0 to 1.0."))
	//The Chance the enemy has to drop an item on death from 0.0 to 1.0
	float ChanceToDrpopItem = 1.0f;

	UPROPERTY(Category = "Enemy", EditAnywhere, BlueprintReadWrite, Replicated, meta = (ExposeOnSpawn = true, NoResetToDefault, DisplayPriority = 0))
	EEnemyType TypeOfEnemy;
	UPROPERTY(VisibleAnywhere, Replicated, NoClear, meta = (DisplayThumbnail = true))
	UWidgetComponent* EnemyHUDWorld;

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Override of AActor's TakeDamage function to handle damage to the enemy."))
	//Override of AActor's TakeDamage function to handle damage to the enemy
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable, meta = (ToolTip = "A method to handle dieing and what happens when the enemy dies.", ShortToolTip = "A method to handle dieing."))
	//A method to handle dieing and what happens when the enemy dies
	virtual void Die(AController* EventInstigator);

	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Check if the enemy is dead.", ShortToolTip = "Check if the enemy is dead."))
	//Check if the enemy is dead
	virtual bool IsDead() { return CurrentHealth <= 0; }

	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Reduces the remaining stun time by DeltaTime. Meant to be called in Tick but can be called elsewhere if needed.", ShortToolTip = "Check if the enemy is dead."))
	//Reduces the remaining stun time by DeltaTime. Meant to be called in Tick but can be called elsewhere if needed.
	void ReduceStunTime(float DeltaTime);

	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Getter for the current health.", ShortToolTip = "Getter for the current health."))
	//Getter for the current health
	const float GetCurrentHealth() { return CurrentHealth; }
	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Getter for the max health.", ShortToolTip = "Getter for the max health."))
	//Getter for the max health
	const float GetMaxHealth() { return MaxHealth; }
	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Getter for the remaining stun time.", ShortToolTip = "Getter for the remaining stun time."))
	//Getter for the remaining stun time
	const float GetStunTime() { return RemainingStunTime; }
	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Getter for the current level.", ShortToolTip = "Getter for the current level."))
	//Getter for the current level
	const int GetEnemyLevel() { return Level; }
	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Getter for the enemy type.", ShortToolTip = "Getter for the enemy type."))
	//Getter for the enemy type
	const EEnemyType GetEnemyType() { return TypeOfEnemy; }
	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Getter for the current resource.", ShortToolTip = "Getter for the current resource."))
	//Getter for the current resource
	int GetEnemyResource() { return EnemyResource; }
	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Getter for the max resource.", ShortToolTip = "Getter for the max resource."))
	//Getter for the max resource
	int GetEnemyMaxResource() { return EnemyMaxResource; }
	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Getter for the enemy name.", ShortToolTip = "Getter for the enemy name."))
	//Getter for the enemy name
	FName GetEnemyName() { return EnemyName; }

	UFUNCTION(BlueprintCallable, BlueprintSetter, meta = (ToolTip = "Getter for the current health.", ShortToolTip = "Getter for the current health."))
	//Setter for the current health
	void SetCurrentHealth(const float& NewHealth);
	UFUNCTION(BlueprintCallable, BlueprintSetter, meta = (ToolTip = "Setter for the max health.", ShortToolTip = "Setter for the max health."))
	//Setter for the max health
	void SetMaxHealth(const float& newHealth);
	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Setter for the remaining stun time.", ShortToolTip = "Setter for the remaining stun time."))
	//Setter for the remaining stun time
	void SetStunTime(const float& newStunTime) { RemainingStunTime = newStunTime; }
	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Setter for the current level.", ShortToolTip = "Setter for the current level."))
	//Setter for the current level
	void SetEnemyLevel(const int& newLevel) { Level = newLevel; }

	

	//Need to be netmulticast to play on all clients as client does own the actor their for client rpc won't work
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void PlayMontageOnClient(UAnimMontage* Montage, float PlayRate = 1.0f);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, meta = (ToolTip = "A method to handle spawning an item on death using infromation from the provided loot pool.", ShortToolTip = "A method to handle spawning an item on death."))
	//A method to handle spawning an item on death using infromation from the provided loot pool
	virtual void SpawnItem();

	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Calculates the amount of XP the enemy should give on death.", ShortToolTip = "Calculates the amount of XP the enemy should giveS."))
	//Calculates the amount of XP the enemy should give on death.
	virtual float CalculateXP() const;

	//rotates hud to face camera, courtesy of Mr big T him self Tristan
	void UpdateEnemyHUDWorldFacing();

	//This is an conditional compilation macro that specifies that the code with in should only be included when compiling for the Unreal Engine Editor. Basically mean it get tosed when building that blasted exe.
#if WITH_EDITOR
	//"Called when a property is changed in the editor" This is a quote from who ever wrote the comment for this function in UObject.h. This method is inherited from that by the way :)
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
