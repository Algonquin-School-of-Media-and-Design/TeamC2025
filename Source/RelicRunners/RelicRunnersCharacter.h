/************************************************************************************
 * Author:   Tristan Anglin
 * Year:     2025
 *
 * Notice:
 *   This source code is the intellectual property of Tristan Anglin.
 *   It is provided solely for use within:
 *     - The Relic Runners personal project
 *     - The Almost A Dozen project
 *
 *   Any use, distribution, or modification outside of these projects
 *   is strictly prohibited without explicit written permission.
 *
 *   © 2025 Tristan Anglin. All rights reserved.
 ************************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "RelicRunnersCharacter.generated.h"

enum class EInventorySorting : uint8;

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ARelicRunnersCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

public:
	ARelicRunnersCharacter();

	//Replication
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	EInventorySorting GetCurrentSortingMethod() const;
	UFUNCTION()
	void OnRep_ChestplateMesh();
	UFUNCTION()
	void OnRep_BootsMesh();
	UFUNCTION()
	void OnRep_LeggingsMesh();
	UFUNCTION()
	void OnRep_ShouldersMesh();
	UFUNCTION()
	void OnRep_HelmetMesh();
	UFUNCTION()
	void OnRep_WaistMesh();
	UFUNCTION()
	void OnRep_BackpackMesh();
	UFUNCTION()
	void OnRep_GlovesMesh();
	UFUNCTION()
	void OnRep_WristMesh();
	UFUNCTION()
	void OnRep_NecklaceMesh();
	UFUNCTION()
	void OnRep_MainhandMesh();
	UFUNCTION()
	void OnRep_OffhandMesh();
	UPROPERTY(ReplicatedUsing = OnRep_ChestplateMesh)
	USkeletalMesh* ReplicatedChestplateMesh;
	UPROPERTY(ReplicatedUsing = OnRep_BootsMesh)
	USkeletalMesh* ReplicatedBootsMesh;
	UPROPERTY(ReplicatedUsing = OnRep_LeggingsMesh)
	USkeletalMesh* ReplicatedLeggingsMesh;
	UPROPERTY(ReplicatedUsing = OnRep_ShouldersMesh)
	USkeletalMesh* ReplicatedShouldersMesh;
	UPROPERTY(ReplicatedUsing = OnRep_HelmetMesh)
	USkeletalMesh* ReplicatedHelmetMesh;
	UPROPERTY(ReplicatedUsing = OnRep_WaistMesh)
	USkeletalMesh* ReplicatedWaistMesh;
	UPROPERTY(ReplicatedUsing = OnRep_BackpackMesh)
	USkeletalMesh* ReplicatedBackpackMesh;
	UPROPERTY(ReplicatedUsing = OnRep_GlovesMesh)
	USkeletalMesh* ReplicatedGlovesMesh;
	UPROPERTY(ReplicatedUsing = OnRep_WristMesh)
	USkeletalMesh* ReplicatedWristMesh;
	UPROPERTY(ReplicatedUsing = OnRep_NecklaceMesh)
	USkeletalMesh* ReplicatedNecklaceMesh;
	UPROPERTY(ReplicatedUsing = OnRep_MainhandMesh)
	UStaticMesh* ReplicatedMainhandMesh;
	UPROPERTY(ReplicatedUsing = OnRep_OffhandMesh)
	UStaticMesh* ReplicatedOffhandMesh;

	//Server
	UFUNCTION(Server, Reliable)
	void Server_SetReplicatedMeshByItemType(UObject* MeshAsset, const FString& ItemType);
	UFUNCTION(Server, Reliable)
	void Server_EquipItemByID(FGuid ID);
	UFUNCTION(Server, Reliable)
	void Server_UnequipItemByID(FGuid ID);
	UFUNCTION(Server, Reliable)
	void Server_SetMaxHealth(int health);
	UFUNCTION(Server, Reliable)
	void Server_DropItem(const struct FItemData& ItemData);
	UFUNCTION(Server, Reliable)
	void Server_PickupItem(AItemActor* TargetItem);
	
	//Client
	UFUNCTION(Client, Reliable)
	void Client_UpdateEquippedStats(const FEquippedStatsSummary& NewStats);

	TSet<AActor*> AlreadyHitActors;
	
	void PerformSwingTrace();

	UFUNCTION(Server, Reliable)
	void Server_HitEnemy(AActor* HitActor);

	//Inputs
	void BasicAttack();
	void Walk(const FInputActionValue& Value);
	void Run(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Interact();
	void InventoryUI();
	void AbilitySystemUI();
	void DamageAbility();
	void DefenceAbility();
	void UtilityAbility();
	void UltimateAbility();
	void HealthPotions();
	void SpendAbilityPoints();

	//Ticking
	void UpdatePlayerHUDWorldFacing();
	void TraceForInteractables();

	//Getters
	int GetPlayerLevel() { return PlayerLevel; };
	int GetPlayerHealth() { return PlayerHealth; }
	int GetPlayerMaxHealth() { return PlayerMaxHealth; }
	int GetPlayerResource() { return PlayerResource; }
	int GetPlayerMaxResource() { return PlayerMaxResource; }
	int GetPlayerXP() { return PlayerXP; }
	int GetPlayerXPToLevel() { return PlayerXPToLevel; }
	int GetPlayerAbilityPoints() { return PlayerAbilityPoints; }

	UFUNCTION(BlueprintCallable, Category = "Stats")
	int GetPlayerStartingMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Stats")
	int GetPlayerNumInventorySlots() const;

	UFUNCTION(BlueprintCallable, Category = "Stats")
	int GetPlayerStartingArmor() const;

	UFUNCTION(BlueprintCallable, Category = "Stats")
	int GetPlayerStartingDexterity() const;

	UFUNCTION(BlueprintCallable, Category = "Stats")
	int GetPlayerStartingStrength() const;

	UFUNCTION(BlueprintCallable, Category = "Stats")
	int GetPlayerStartingIntelligence() const;

	UFUNCTION(BlueprintCallable, Category = "Stats")
	int GetPlayerStartingLuck() const;

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void SetPlayerStartingArmor(int StartingArmor) { PlayerStartingArmor =  StartingArmor; }

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void SetPlayerStartingDexterity(int StartingDexterity) { PlayerStartingDexterity = StartingDexterity; }

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void SetPlayerStartingIntelligence(int StartingIntelligence) { PlayerStartingIntelligence = StartingIntelligence; }

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void SetPlayerStartingLuck(int StartingLuck) { PlayerStartingLuck = StartingLuck; }

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void SetPlayerStartingHealth(int StartingHealth) { PlayerStartingMaxHealth = StartingHealth; }

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void SetPlayerStartingStrength(int StartingStrength) { PlayerStartingStrength = StartingStrength; }


	class UInventory* GetInventory() { return Inventory; };
	class UInventoryComponent* GetInventoryComponent() { return InventoryComponent; };

	//Update functions
	void UpdateItemVisuals(UObject* MeshAsset, const FString& ItemType);
	void UpdateItemVisuals(class UItemObject* Item);
	void UpdateHUD();

	//Helper functions
	FString GetMeshPathByItemType(const FString& ItemType);
	USkeletalMeshComponent* GetSkeletalMeshComponentByItemType(const FString& ItemType);
	UStaticMeshComponent* GetStaticMeshComponentByItemType(const FString& ItemType);
	void SetReplicatedMeshByItemType(UObject* MeshAsset, const FString& ItemType);
	void OnRep_MeshUpdate(UObject* MeshAsset, const FString& ItemType);

	void TryBindInventoryDelegates();
	
	//Animations
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimSequence* SwordBasicAttack0;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimSequence* SwordBasicAttack1;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimSequence* SwordBasicAttack2;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimSequence* SwordBasicAttack3;

	//Meshes
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ChestplateMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* BootsMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* LeggingsMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ShouldersMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* HelmetMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WaistMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* BackpackMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* GlovesMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WristMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* NecklaceMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MainhandItemMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* OffhandItemMesh;

	//Other Classes
	UPROPERTY()
	class UItemMeshData* ItemMeshData;
	UPROPERTY(EditDefaultsOnly, Category = "Item Generation")
	TSubclassOf<class UItemMeshData> ItemMeshDataClass;

	UPROPERTY()
	class UInventory* Inventory;

	void AddExperience(int Amount);

	void OnLevelUp();

protected:
	
	//Other Classes
	UPROPERTY(EditDefaultsOnly, Category = "ItemActor")
	TSubclassOf<class AItemActor> ItemActorClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UInventory> InventoryClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UPlayerHUD> PlayerHUDClass;
	UPROPERTY()
	class UPlayerHUD* PlayerHUD;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UAbilityPointCounter> AbilityPointCounterClass;

	UPROPERTY()
	class UAbilityPointCounter* AbilityPointCounter;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UHealthPotion> HealthPotionClass;

	UPROPERTY()
	class UHealthPotion* HealthPotion;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UAbilitySelection> AbilitySelectionClass;

	UPROPERTY()
	class UAbilitySelection* AbilitySelection;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ULevelUpHUD> LevelUpHUDClass;
	UPROPERTY()
	class ULevelUpHUD* LevelUpHUD;

	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* PlayerHUDWorld;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	class UInventoryComponent* InventoryComponent;

	FTimerHandle DelegateBindHandle;

	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateHUD();

	virtual void OnRep_PlayerState() override;

	//Replication
	UFUNCTION()
	void OnRep_HUD();

	UFUNCTION()
	void OnRep_AbilityPoints();

	//Stats
	UPROPERTY()
	int PlayerNumInventorySlots;
	UPROPERTY(ReplicatedUsing = OnRep_HUD)
	int PlayerHealth;
	UPROPERTY(ReplicatedUsing = OnRep_HUD)
	int PlayerMaxHealth;
	UPROPERTY(ReplicatedUsing = OnRep_HUD)
	int PlayerResource;
	UPROPERTY(ReplicatedUsing = OnRep_HUD)
	int PlayerMaxResource;
	UPROPERTY(ReplicatedUsing = OnRep_HUD)
	int PlayerXP;
	UPROPERTY(ReplicatedUsing = OnRep_HUD)
	int PlayerXPToLevel;
	UPROPERTY(ReplicatedUsing = OnRep_HUD)
	int PlayerLevel;
	UPROPERTY(ReplicatedUsing = OnRep_AbilityPoints)
	int PlayerAbilityPoints;
	UPROPERTY()
	int PlayerArmor;
	UPROPERTY()
	int PlayerDexterity;
	UPROPERTY()
	int PlayerStrength;
	UPROPERTY()
	int PlayerIntelligence;
	UPROPERTY()
	int PlayerLuck;
	UPROPERTY()
	int PlayerStartingMaxHealth;
	UPROPERTY()
	int PlayerStartingMaxResource;
	UPROPERTY()
	int PlayerStartingArmor;
	UPROPERTY()
	int PlayerStartingDexterity;
	UPROPERTY()
	int PlayerStartingStrength;
	UPROPERTY()
	int PlayerStartingIntelligence;
	UPROPERTY()
	int PlayerStartingLuck;

	UPROPERTY()
	int HealthPotionCount;   
	UPROPERTY()
	int HealthGranted;         

	float DamageCooldown = 5.f;
	float DefenceCooldown = 5.f;
	float UtilityCooldown = 5.f;
	float UltimateCooldown = 10.f;

	//Setup
	virtual void BeginPlay();
	void InitLocalUI();

	void SpawnStarterItems();

	//Ticking
	void Tick(float DeltaTime);
	UFUNCTION()
	void PassiveHealthRegen();
	FTimerHandle HealthRegenTimerHandle;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
};

