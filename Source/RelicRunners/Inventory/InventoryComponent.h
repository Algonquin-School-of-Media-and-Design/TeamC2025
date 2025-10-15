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
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

enum class EInventorySorting : uint8;

class UItemObject;

USTRUCT(BlueprintType)
struct FEquippedStatsSummary
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly) int TotalHealth = 0;
    UPROPERTY(BlueprintReadOnly) int TotalArmor = 0;
    UPROPERTY(BlueprintReadOnly) int TotalDexterity = 0;
    UPROPERTY(BlueprintReadOnly) int TotalStrength = 0;
    UPROPERTY(BlueprintReadOnly) int TotalIntelligence = 0;
    UPROPERTY(BlueprintReadOnly) int TotalLuck = 0;
    UPROPERTY(BlueprintReadOnly) int TotalSlots = 0;
};

USTRUCT(BlueprintType)
struct FEquippedItemEntry
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    FString ItemType;

    UPROPERTY(BlueprintReadWrite)
    UItemObject* Item = nullptr;

    FEquippedItemEntry() {}

    FEquippedItemEntry(const FString& InItemType, UItemObject* InItem)
        : ItemType(InItemType), Item(InItem) {}
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class RELICRUNNERS_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInventoryComponent();

    // Inventory
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Inventory, Category="Inventory", meta=(AllowPrivateAccess = "true"))
    TArray<UItemObject*> InventoryItems;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_EquippedItems, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
    TArray<FEquippedItemEntry> EquippedItems;

    TArray<FEquippedItemEntry> GetEquippedItems() { return EquippedItems; };

    // Inventory logic
    UFUNCTION(BlueprintCallable)
    void AddItem(UItemObject* Item);

    void SortInventoryByCurrentMethod();

    UFUNCTION(Server, Reliable)
    void Server_SortInventory(EInventorySorting NewMethod);

    void SetSortingMethod(EInventorySorting NewMethod);

    void HandleSortingNow();

    UFUNCTION(BlueprintCallable)
    void EquipItem(UItemObject* Item);

    UFUNCTION()
    void OnRep_Inventory();

    UFUNCTION()
    void OnRep_EquippedItems();

    UFUNCTION(BlueprintCallable)
    void UnequipItem(UItemObject* Item);

    // Access equipped item
    UFUNCTION(BlueprintCallable)
    UItemObject* GetEquippedItemByType(const FString& ItemType) const;
    UFUNCTION(BlueprintCallable)
    UItemObject*& GetEquippedItemReference(const FString& ItemType);

    void UpdateTotalEquippedStats(class ARelicRunnersCharacter* Char);

    void UpdateTotalEquippedStats(class AEnemyCharacterAI* Char);

    // Stats summary
    FEquippedStatsSummary CalculateEquippedStats() const;

    FEquippedStatsSummary CachedEquippedStats;
    FEquippedStatsSummary GetCachedEquippedStats() { return CachedEquippedStats; }

    // Delegates
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatsChanged, const FEquippedStatsSummary&, NewStats);
    UPROPERTY(BlueprintAssignable)
    FOnStatsChanged OnStatsChanged;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);
    UPROPERTY(BlueprintAssignable)
    FOnInventoryChanged OnInventoryChanged;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquipmentChanged);
    UPROPERTY(BlueprintAssignable)
    FOnEquipmentChanged OnEquipmentChanged;

    // Replication
    UPROPERTY(ReplicatedUsing = OnRep_SortingMethod)
    EInventorySorting CurrentSortingMethod;
    
    UFUNCTION()
    void OnRep_SortingMethod();

    EInventorySorting GetCurrentSortingMethod() { return CurrentSortingMethod; }

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    virtual void BeginPlay() override;
    virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
    
};