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
#include "Blueprint/UserWidget.h"
#include "InventoryToolTip.generated.h"

/**
 * 
 */
UCLASS()
class RELICRUNNERS_API UInventoryToolTip : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_LevelTooltip;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_RarityTooltip;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_ItemTypeTooltip;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_HealthTooltip;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_ArmorTooltip;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_DexterityTooltip;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_StrengthTooltip;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_IntelligenceTooltip;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_LuckTooltip;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_SlotsTooltip;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_FromWhere;

	UPROPERTY(meta = (BindWidget))
	class UBorder* B_Rarity;

	UPROPERTY(meta = (BindWidget))
	class UBorder* B_ItemType;

	UPROPERTY(meta = (BindWidget))
	class UBorder* B_Level;

	UPROPERTY(meta = (BindWidget))
	class UBorder* B_Health;

	UPROPERTY(meta = (BindWidget))
	class UBorder* B_Armor;

	UPROPERTY(meta = (BindWidget))
	class UBorder* B_Dexterity;

	UPROPERTY(meta = (BindWidget))
	class UBorder* B_Strength;

	UPROPERTY(meta = (BindWidget))
	class UBorder* B_Intelligence;

	UPROPERTY(meta = (BindWidget))
	class UBorder* B_Luck;

	UPROPERTY(meta = (BindWidget))
	class UBorder* B_Slots;

	UPROPERTY(meta = (BindWidget))
	class UBorder* B_FromWhere;

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* HB_Health;

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* HB_Armor;

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* HB_Dexterity;

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* HB_Strength;

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* HB_Intelligence;

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* HB_Luck;

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* HB_Slots;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_LevelTooltip_1;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_RarityTooltip_1;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_ItemTypeTooltip_1;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_HealthTooltip_1;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_HealthTooltip_2;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_ArmorTooltip_1;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_ArmorTooltip_2;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_DexterityTooltip_1;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_DexterityTooltip_2;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_StrengthTooltip_1;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_StrengthTooltip_2;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_IntelligenceTooltip_1;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_IntelligenceTooltip_2;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_LuckTooltip_1;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_LuckTooltip_2;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_SlotsTooltip_1;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_SlotsTooltip_2;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_FromWhere_1;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_FromWhere_2;

	UPROPERTY(meta = (BindWidget))
	class UBorder* B_Rarity_1;

	UPROPERTY(meta = (BindWidget))
	class UBorder* B_ItemType_1;

	UPROPERTY(meta = (BindWidget))
	class UBorder* B_Level_1;

	UPROPERTY(meta = (BindWidget))
	class UBorder* B_Health_1;

	UPROPERTY(meta = (BindWidget))
	class UBorder* B_Health_2;

	UPROPERTY(meta = (BindWidget))
	class UBorder* B_Armor_1;

	UPROPERTY(meta = (BindWidget))
	class UBorder* B_Armor_2;

	UPROPERTY(meta = (BindWidget))
	class UBorder* B_Dexterity_1;

	UPROPERTY(meta = (BindWidget))
	class UBorder* B_Dexterity_2;

	UPROPERTY(meta = (BindWidget))
	class UBorder* B_Strength_1;

	UPROPERTY(meta = (BindWidget))
	class UBorder* B_Strength_2;

	UPROPERTY(meta = (BindWidget))
	class UBorder* B_Intelligence_1;

	UPROPERTY(meta = (BindWidget))
	class UBorder* B_Intelligence_2;

	UPROPERTY(meta = (BindWidget))
	class UBorder* B_Luck_1;

	UPROPERTY(meta = (BindWidget))
	class UBorder* B_Luck_2;

	UPROPERTY(meta = (BindWidget))
	class UBorder* B_Slots_1;

	UPROPERTY(meta = (BindWidget))
	class UBorder* B_Slots_2;

	UPROPERTY(meta = (BindWidget))
	class UBorder* B_FromWhere_1;

	UPROPERTY(meta = (BindWidget))
	class UBorder* B_FromWhere_2;

	UPROPERTY(meta = (BindWidget))
	class UBorder* B_Equipped;

	UPROPERTY(meta = (BindWidget))
	class UBorder* B_Comparison;

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* HB_Health_1;

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* HB_Health_2;

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* HB_Armor_1;

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* HB_Armor_2;

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* HB_Dexterity_1;

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* HB_Dexterity_2;

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* HB_Strength_1;

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* HB_Strength_2;

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* HB_Intelligence_1;

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* HB_Intelligence_2;

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* HB_Luck_1;

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* HB_Luck_2;

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* HB_Slots_1;

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* HB_Slots_2;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void Setup(struct FItemData& ItemData, const FLinearColor& RarityColor);
	void Setup(struct FItemData& ItemData, const FLinearColor& RarityColor, struct FItemData& ItemDataEquipped);
	void Setup(class UItemObject* HoveredItem, const FLinearColor& HoveredColor, class UItemObject* EquippedItem);
};
