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

#include "InventorySortingOptions.h"
#include "Components/Button.h"

FString UInventorySortingOptions::GetSortingMethodDisplayName(EInventorySorting Method)
{
	switch (Method)
	{
	case EInventorySorting::SortByItemType:		return TEXT("Item Type");
	case EInventorySorting::SortByRarity:		return TEXT("Rarity");
	case EInventorySorting::SortByLevel:		return TEXT("Level");
	case EInventorySorting::SortByHealth:		return TEXT("Health");
	case EInventorySorting::SortByArmor:		return TEXT("Armor");
	case EInventorySorting::SortByDexterity:    return TEXT("Dexterity");
	case EInventorySorting::SortByStrength:		return TEXT("Strength");
	case EInventorySorting::SortByIntelligence: return TEXT("Intelligence");
	case EInventorySorting::SortByLuck:			return TEXT("Luck");
	case EInventorySorting::SortBySlots:		return TEXT("Slots");
	default: return TEXT("Sort");
	}
}

void UInventorySortingOptions::NativeConstruct()
{
	Super::NativeConstruct();

	B_SortByType->OnClicked.AddDynamic(this, &UInventorySortingOptions::SortByItemTypeClicked);
	B_SortByRarity->OnClicked.AddDynamic(this, &UInventorySortingOptions::SortByRarityClicked);
	B_SortByLevel->OnClicked.AddDynamic(this, &UInventorySortingOptions::SortByLevelClicked);
	B_SortByHealth->OnClicked.AddDynamic(this, &UInventorySortingOptions::SortByHealthClicked);
	B_SortByArmor->OnClicked.AddDynamic(this, &UInventorySortingOptions::SortByArmorClicked);
	B_SortByDexterity->OnClicked.AddDynamic(this, &UInventorySortingOptions::SortByDexterityClicked);
	B_SortByStrength->OnClicked.AddDynamic(this, &UInventorySortingOptions::SortByStrengthClicked);
	B_SortByIntelligence->OnClicked.AddDynamic(this, &UInventorySortingOptions::SortByIntelligenceClicked);
	B_SortByLuck->OnClicked.AddDynamic(this, &UInventorySortingOptions::SortByLuckClicked);
	B_SortBySlots->OnClicked.AddDynamic(this, &UInventorySortingOptions::SortBySlotsClicked);
}

void UInventorySortingOptions::SortByItemTypeClicked()
{
	OnSortingSelected.Broadcast(EInventorySorting::SortByItemType);
}

void UInventorySortingOptions::SortByRarityClicked()
{
	OnSortingSelected.Broadcast(EInventorySorting::SortByRarity);
}

void UInventorySortingOptions::SortByLevelClicked()
{
	OnSortingSelected.Broadcast(EInventorySorting::SortByLevel);
}

void UInventorySortingOptions::SortByHealthClicked()
{
	OnSortingSelected.Broadcast(EInventorySorting::SortByHealth);
}

void UInventorySortingOptions::SortByArmorClicked()
{
	OnSortingSelected.Broadcast(EInventorySorting::SortByArmor);
}

void UInventorySortingOptions::SortByDexterityClicked()
{
	OnSortingSelected.Broadcast(EInventorySorting::SortByDexterity);
}

void UInventorySortingOptions::SortByStrengthClicked()
{
	OnSortingSelected.Broadcast(EInventorySorting::SortByStrength);
}

void UInventorySortingOptions::SortByIntelligenceClicked()
{
	OnSortingSelected.Broadcast(EInventorySorting::SortByIntelligence);
}

void UInventorySortingOptions::SortByLuckClicked()
{
	OnSortingSelected.Broadcast(EInventorySorting::SortByLuck);
}

void UInventorySortingOptions::SortBySlotsClicked()
{
	OnSortingSelected.Broadcast(EInventorySorting::SortBySlots);
}