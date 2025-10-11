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

#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Inventory.h"
#include "InventorySortingOptions.h"
#include "RelicRunners/Item/ItemData.h"
#include "RelicRunners/RelicRunnersCharacter.h"
#include "RelicRunners/Enemy/EnemyCharacterAI.h"
#include "Engine/ActorChannel.h"
#include "RelicRunners/Item/ItemStats.h"
#include <RelicRunners/PlayerController/RelicRunnersPlayerController.h>

UInventoryComponent::UInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("InventoryComponent BeginPlay on %s (Role: %d)"), *GetOwner()->GetName(), (int32)GetOwnerRole());
}

bool UInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
    bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

    for (UItemObject* Item : InventoryItems)
    {
        if (Item)
        {
            WroteSomething |= Channel->ReplicateSubobject(Item, *Bunch, *RepFlags);
        }
    }

    for (const FEquippedItemEntry& Entry : EquippedItems)
    {
        if (Entry.Item)
        {
            WroteSomething |= Channel->ReplicateSubobject(Entry.Item, *Bunch, *RepFlags);
        }
    }

    return WroteSomething;
}

void UInventoryComponent::AddItem(UItemObject* Item)
{
    if (!Item) return;

    InventoryItems.Add(Item);
    SortInventoryByCurrentMethod();
    OnInventoryChanged.Broadcast();
}

void UInventoryComponent::OnRep_SortingMethod()
{
    // This runs on clients when CurrentSortingMethod is replicated
    HandleSortingNow(); // Show sorted inventory on client UI
}

void UInventoryComponent::SetSortingMethod(EInventorySorting NewMethod)
{
    if (CurrentSortingMethod == NewMethod)
        return;

    CurrentSortingMethod = NewMethod;

    if (!GetOwner()->HasAuthority())
    {
        // Client requests the server to sort
        Server_SortInventory(NewMethod);
        return;
    }

    // Server sorts immediately
    SortInventoryByCurrentMethod();
}

void UInventoryComponent::SortInventoryByCurrentMethod()
{
    if (!GetOwner() || !GetOwner()->HasAuthority())
    {
        return;
    }

    HandleSortingNow();
    OnInventoryChanged.Broadcast(); // Notify UI listeners
}

void UInventoryComponent::Server_SortInventory_Implementation(EInventorySorting Method)
{
    // Update sort method and sort server-side
    SetSortingMethod(Method);
}

void UInventoryComponent::HandleSortingNow()
{
    TArray<UItemObject*>& Items = InventoryItems;

    const TMap<FString, ItemStats::FRarityData>& RarityMap = ItemStats::GetRarityDataMap();
    TMap<FString, int32> RarityRank;

    int32 Index = 0;
    for (const auto& Elem : RarityMap)
    {
        RarityRank.Add(Elem.Key, Index++);
    }

    switch (CurrentSortingMethod)
    {
    case EInventorySorting::SortByItemType:
        Items.Sort([&RarityRank](UItemObject& A, UItemObject& B)
            {
                if (A.ItemData.ItemType != B.ItemData.ItemType)
                    return A.ItemData.ItemType < B.ItemData.ItemType;

                int32 ARank = RarityRank.FindRef(A.ItemData.Rarity);
                int32 BRank = RarityRank.FindRef(B.ItemData.Rarity);

                if (ARank != BRank)
                    return ARank > BRank;

                return A.ItemData.Level > B.ItemData.Level;
            });
        break;

    case EInventorySorting::SortByRarity:
        Items.Sort([&RarityRank](UItemObject& A, UItemObject& B)
            {
                int32 ARank = RarityRank.FindRef(A.ItemData.Rarity);
                int32 BRank = RarityRank.FindRef(B.ItemData.Rarity);

                if (ARank != BRank)
                    return ARank > BRank;

                if (A.ItemData.Level != B.ItemData.Level)
                    return A.ItemData.Level > B.ItemData.Level;

                return A.ItemData.ItemType < B.ItemData.ItemType;
            });
        break;

    case EInventorySorting::SortByLevel:
        Items.Sort([&RarityRank](UItemObject& A, UItemObject& B)
            {
                if (A.ItemData.Level != B.ItemData.Level)
                    return A.ItemData.Level > B.ItemData.Level;

                int32 ARank = RarityRank.FindRef(A.ItemData.Rarity);
                int32 BRank = RarityRank.FindRef(B.ItemData.Rarity);

                if (ARank != BRank)
                    return ARank > BRank;

                return A.ItemData.ItemType < B.ItemData.ItemType;
            });
        break;

    case EInventorySorting::SortByHealth:
        Items.Sort([&RarityRank](UItemObject& A, UItemObject& B)
            {
                if (A.ItemData.Health != B.ItemData.Health)
                    return A.ItemData.Health > B.ItemData.Health;

                int32 ARank = RarityRank.FindRef(A.ItemData.Rarity);
                int32 BRank = RarityRank.FindRef(B.ItemData.Rarity);

                if (ARank != BRank)
                    return ARank > BRank;

                return A.ItemData.ItemType < B.ItemData.ItemType;
            });
        break;

    case EInventorySorting::SortByArmor:
        Items.Sort([&RarityRank](UItemObject& A, UItemObject& B)
            {
                if (A.ItemData.Armor != B.ItemData.Armor)
                    return A.ItemData.Armor > B.ItemData.Armor;

                int32 ARank = RarityRank.FindRef(A.ItemData.Rarity);
                int32 BRank = RarityRank.FindRef(B.ItemData.Rarity);

                if (ARank != BRank)
                    return ARank > BRank;

                return A.ItemData.ItemType < B.ItemData.ItemType;
            });
        break;

    case EInventorySorting::SortByDexterity:
        Items.Sort([&RarityRank](UItemObject& A, UItemObject& B)
            {
                if (A.ItemData.Dexterity != B.ItemData.Dexterity)
                    return A.ItemData.Dexterity > B.ItemData.Dexterity;

                int32 ARank = RarityRank.FindRef(A.ItemData.Rarity);
                int32 BRank = RarityRank.FindRef(B.ItemData.Rarity);

                if (ARank != BRank)
                    return ARank > BRank;

                return A.ItemData.ItemType < B.ItemData.ItemType;
            });
        break;

    case EInventorySorting::SortByStrength:
        Items.Sort([&RarityRank](UItemObject& A, UItemObject& B)
            {
                if (A.ItemData.Strength != B.ItemData.Strength)
                    return A.ItemData.Strength > B.ItemData.Strength;

                int32 ARank = RarityRank.FindRef(A.ItemData.Rarity);
                int32 BRank = RarityRank.FindRef(B.ItemData.Rarity);

                if (ARank != BRank)
                    return ARank > BRank;

                return A.ItemData.ItemType < B.ItemData.ItemType;
            });
        break;

    case EInventorySorting::SortByIntelligence:
        Items.Sort([&RarityRank](UItemObject& A, UItemObject& B)
            {
                if (A.ItemData.Intelligence != B.ItemData.Intelligence)
                    return A.ItemData.Intelligence > B.ItemData.Intelligence;

                int32 ARank = RarityRank.FindRef(A.ItemData.Rarity);
                int32 BRank = RarityRank.FindRef(B.ItemData.Rarity);

                if (ARank != BRank)
                    return ARank > BRank;

                return A.ItemData.ItemType < B.ItemData.ItemType;
            });
        break;

    case EInventorySorting::SortByLuck:
        Items.Sort([&RarityRank](UItemObject& A, UItemObject& B)
            {
                if (A.ItemData.Luck != B.ItemData.Luck)
                    return A.ItemData.Luck > B.ItemData.Luck;

                int32 ARank = RarityRank.FindRef(A.ItemData.Rarity);
                int32 BRank = RarityRank.FindRef(B.ItemData.Rarity);

                if (ARank != BRank)
                    return ARank > BRank;

                return A.ItemData.ItemType < B.ItemData.ItemType;
            });
        break;

    case EInventorySorting::SortBySlots:
        Items.Sort([&RarityRank](UItemObject& A, UItemObject& B)
            {
                if (A.ItemData.Slots != B.ItemData.Slots)
                    return A.ItemData.Slots > B.ItemData.Slots;

                int32 ARank = RarityRank.FindRef(A.ItemData.Rarity);
                int32 BRank = RarityRank.FindRef(B.ItemData.Rarity);

                if (ARank != BRank)
                    return ARank > BRank;

                return A.ItemData.ItemType < B.ItemData.ItemType;
            });
        break;

    default:
        break;
    }

    OnInventoryChanged.Broadcast(); // Update UI after sorting
}

void UInventoryComponent::EquipItem(UItemObject* Item)
{
    if (!Item) return;

    const FString Slot = Item->ItemData.ItemType;
    bool bFound = false;
    
    for (FEquippedItemEntry& Entry : EquippedItems)
    {
        if (Entry.ItemType == Slot)
        {
            if (Entry.Item)
            {
                AddItem(Entry.Item); // Add previous item to inventory
            }
            Entry.Item = Item;
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        EquippedItems.Add(FEquippedItemEntry(Slot, Item));
    }

    int Index = InventoryItems.IndexOfByPredicate([&](const UItemObject* OtherItem)
        {
            return OtherItem && OtherItem->ItemData.UniqueID == Item->ItemData.UniqueID;
        });

    if (Index != INDEX_NONE)
    {
        InventoryItems.RemoveAt(Index);
    }

    if (ARelicRunnersPlayerController* PC = Cast<ARelicRunnersPlayerController>(GetWorld()->GetFirstPlayerController()))
    {
        PC->UpdatePreviewWithEquippedItems();
    }

    OnEquipmentChanged.Broadcast();
    OnInventoryChanged.Broadcast();
}

void UInventoryComponent::OnRep_Inventory()
{
    OnInventoryChanged.Broadcast();
}

void UInventoryComponent::OnRep_EquippedItems()
{

    OnEquipmentChanged.Broadcast();
}

void UInventoryComponent::UnequipItem(UItemObject* Item)
{
    if (!Item) return;

    for (int32 i = 0; i < EquippedItems.Num(); ++i)
    {
        if (EquippedItems[i].Item == nullptr) continue;

        if (EquippedItems[i].Item->GetUniqueID() == Item->GetUniqueID())
        {
            InventoryItems.Add(Item);
            EquippedItems.RemoveAt(i);

            OnEquipmentChanged.Broadcast();
            OnInventoryChanged.Broadcast();
            return;
        }
    }

    if (ARelicRunnersPlayerController* PC = Cast<ARelicRunnersPlayerController>(GetWorld()->GetFirstPlayerController()))
    {
        PC->UpdatePreviewWithEquippedItems();
    }
}

UItemObject* UInventoryComponent::GetEquippedItemByType(const FString& ItemType) const
{
    for (const FEquippedItemEntry& Entry : EquippedItems)
    {
        if (Entry.ItemType == ItemType)
        {
            return Entry.Item;
        }
    }
    return nullptr;
}

UItemObject*& UInventoryComponent::GetEquippedItemReference(const FString& ItemType)
{
    for (FEquippedItemEntry& Entry : EquippedItems)
    {
        if (Entry.ItemType == ItemType)
        {
            return Entry.Item;
        }
    }

    // Add and return new reference
    EquippedItems.Add(FEquippedItemEntry(ItemType, nullptr));
    return EquippedItems.Last().Item;
}

void UInventoryComponent::UpdateTotalEquippedStats(ARelicRunnersCharacter* Char)
{
    if (!Char) return;

    FEquippedStatsSummary Stats = CalculateEquippedStats();

    if (Char->HasAuthority())
    {
        Char->Server_SetMaxHealth(Stats.TotalHealth);

        // Notify owning client
        Char->Client_UpdateEquippedStats(Stats);
    }

    // Store for local access/UI
    CachedEquippedStats = Stats;
    OnStatsChanged.Broadcast(Stats); // Works only on server/UI
}

void UInventoryComponent::UpdateTotalEquippedStats(AEnemyCharacterAI* Char)
{
    if (!Char) return;

    FEquippedStatsSummary Stats = CalculateEquippedStats();

    if (Char->HasAuthority())
    {
        Char->SetMaxHealth(Stats.TotalHealth);
    }

    Char->Client_UpdateEquippedStats(Stats);

    CachedEquippedStats = Stats;
    OnStatsChanged.Broadcast(Stats);
}

FEquippedStatsSummary UInventoryComponent::CalculateEquippedStats() const
{
    FEquippedStatsSummary Summary;

    if (const ARelicRunnersCharacter* OwnerChar = Cast<ARelicRunnersCharacter>(GetOwner()))
    {
        Summary.TotalHealth = OwnerChar->GetPlayerStartingMaxHealth();
        Summary.TotalSlots = OwnerChar->GetPlayerNumInventorySlots();
        Summary.TotalArmor = OwnerChar->GetPlayerStartingArmor();
        Summary.TotalDexterity = OwnerChar->GetPlayerStartingDexterity();
        Summary.TotalStrength = OwnerChar->GetPlayerStartingStrength();
        Summary.TotalIntelligence = OwnerChar->GetPlayerStartingIntelligence();
        Summary.TotalLuck = OwnerChar->GetPlayerStartingLuck();
    }

    for (const FEquippedItemEntry& Entry : EquippedItems)
    {
        if (UItemObject* Item = Entry.Item)
        {
            Summary.TotalHealth += Item->ItemData.Health;
            Summary.TotalArmor += Item->ItemData.Armor;
            Summary.TotalDexterity += Item->ItemData.Dexterity;
            Summary.TotalStrength += Item->ItemData.Strength;
            Summary.TotalIntelligence += Item->ItemData.Intelligence;
            Summary.TotalLuck += Item->ItemData.Luck;
            Summary.TotalSlots += Item->ItemData.Slots;
        }
    }

    return Summary;
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UInventoryComponent, InventoryItems);
    DOREPLIFETIME(UInventoryComponent, EquippedItems);
    DOREPLIFETIME(UInventoryComponent, CurrentSortingMethod);

}