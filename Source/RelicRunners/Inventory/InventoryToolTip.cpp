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

#include "InventoryToolTip.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/HorizontalBox.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CanvasPanelSlot.h"
#include "RelicRunners/Item/ItemData.h"
#include "InventoryComponent.h"
#include <RelicRunners/Item/ItemStats.h>

void UInventoryToolTip::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    // We can include our changes to the widget text here
}

void UInventoryToolTip::Setup(FItemData& ItemData, const FLinearColor& RarityColor)
{
    UItemObject* Item = NewObject<UItemObject>(this);
    Item->SetItemData(ItemData);

    Setup(Item, RarityColor, nullptr);
}

void UInventoryToolTip::Setup(UItemObject* HoveredItem, const FLinearColor& HoveredColor, UItemObject* EquippedItem)
{
    // --- Hovered item UI ---
    B_Rarity->SetBrushColor(HoveredColor);
    TB_RarityTooltip->SetText(FText::FromString(HoveredItem->GetRarity()));
    TB_RarityTooltip->SetColorAndOpacity(FLinearColor::White);

    TB_ItemTypeTooltip->SetText(FText::FromString(HoveredItem->GetItemType()));
    B_ItemType->SetBrushColor(FLinearColor(0.2f, 0.2f, 0.2f, 1.0f));

    TB_LevelTooltip->SetText(FText::FromString("Level " + FString::FromInt(HoveredItem->GetLevel())));
    TB_LevelTooltip->SetColorAndOpacity(FLinearColor::White);
    B_Level->SetBrushColor(FLinearColor(0.2f, 0.2f, 0.2f, 1.0f));

    const bool bHasEquipped = EquippedItem != nullptr;

    // --- Equipped item UI (note the _1 suffixes) ---
    if (bHasEquipped)
    {
        FLinearColor EquippedColor = ItemStats::GetRarityDataMap()[EquippedItem->GetRarity()].Color;

        B_Rarity_1->SetBrushColor(EquippedColor);
        TB_RarityTooltip_1->SetText(FText::FromString(EquippedItem->GetRarity()));
        TB_RarityTooltip_1->SetColorAndOpacity(FLinearColor::White);

        TB_ItemTypeTooltip_1->SetText(FText::FromString(EquippedItem->GetItemType()));
        B_ItemType_1->SetBrushColor(FLinearColor(0.2f, 0.2f, 0.2f, 1.0f));

        TB_LevelTooltip_1->SetText(FText::FromString("Level " + FString::FromInt(EquippedItem->GetLevel())));
        TB_LevelTooltip_1->SetColorAndOpacity(FLinearColor::White);
        B_Level_1->SetBrushColor(FLinearColor(0.2f, 0.2f, 0.2f, 1.0f));
    }
    else
    {
        B_Rarity_1->SetBrushColor(FLinearColor::Transparent);
        TB_RarityTooltip_1->SetText(FText::GetEmpty());

        TB_ItemTypeTooltip_1->SetText(FText::GetEmpty());
        B_ItemType_1->SetBrushColor(FLinearColor::Transparent);

        TB_LevelTooltip_1->SetText(FText::GetEmpty());
        B_Level_1->SetBrushColor(FLinearColor::Transparent);
    }

    struct FStatInfo
    {
        int32 Value;
        FString Label;
        UTextBlock* HoveredText;
        UHorizontalBox* HoveredBox;
        UBorder* HoveredBorder;
        UTextBlock* EquippedText;
        UHorizontalBox* EquippedBox;
        UBorder* EquippedBorder;
        UTextBlock* ComparisonText;
        UHorizontalBox* ComparisonBox;
        UBorder* ComparisonBorder;
    };

    TArray<FStatInfo> Stats = {
        { HoveredItem->ItemData.Health,       "Health",       TB_HealthTooltip,       HB_Health,       B_Health,       TB_HealthTooltip_1,       HB_Health_1,       B_Health_1,       TB_HealthTooltip_2,       HB_Health_2,        B_Health_2 },
        { HoveredItem->ItemData.Armor,        "Armor",        TB_ArmorTooltip,        HB_Armor,        B_Armor,        TB_ArmorTooltip_1,        HB_Armor_1,        B_Armor_1,        TB_ArmorTooltip_2,        HB_Armor_2,         B_Armor_2 },
        { HoveredItem->ItemData.Dexterity,    "Dexterity",    TB_DexterityTooltip,    HB_Dexterity,    B_Dexterity,    TB_DexterityTooltip_1,    HB_Dexterity_1,    B_Dexterity_1,    TB_DexterityTooltip_2,    HB_Dexterity_2,     B_Dexterity_2 },
        { HoveredItem->ItemData.Strength,     "Strength",     TB_StrengthTooltip,     HB_Strength,     B_Strength,     TB_StrengthTooltip_1,     HB_Strength_1,     B_Strength_1,     TB_StrengthTooltip_2,     HB_Strength_2,      B_Strength_2 },
        { HoveredItem->ItemData.Intelligence, "Intelligence", TB_IntelligenceTooltip, HB_Intelligence, B_Intelligence, TB_IntelligenceTooltip_1, HB_Intelligence_1, B_Intelligence_1, TB_IntelligenceTooltip_2, HB_Intelligence_2,  B_Intelligence_2 },
        { HoveredItem->ItemData.Luck,         "Luck",         TB_LuckTooltip,         HB_Luck,         B_Luck,         TB_LuckTooltip_1,         HB_Luck_1,         B_Luck_1,         TB_LuckTooltip_2,         HB_Luck_2,          B_Luck_2 },
        { HoveredItem->ItemData.Slots,        "Slots",        TB_SlotsTooltip,        HB_Slots,        B_Slots,        TB_SlotsTooltip_1,        HB_Slots_1,        B_Slots_1,        TB_SlotsTooltip_2,        HB_Slots_2,         B_Slots_2 }
    };

    // Set visibility of Equipped and Comparison sections
    B_Equipped->SetVisibility(bHasEquipped ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    B_Comparison->SetVisibility(bHasEquipped ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

    for (const FStatInfo& Stat : Stats)
    {
        // Hovered (base column)
        if (Stat.Value == 0)
        {
            Stat.HoveredBox->SetVisibility(ESlateVisibility::Collapsed);
            Stat.HoveredBorder->SetVisibility(ESlateVisibility::Collapsed);
        }
        else
        {
            Stat.HoveredBox->SetVisibility(ESlateVisibility::Visible);
            Stat.HoveredBorder->SetVisibility(ESlateVisibility::Visible);
            Stat.HoveredText->SetText(FText::FromString("+" + FString::FromInt(Stat.Value) + " " + Stat.Label));
            Stat.HoveredText->SetColorAndOpacity(FLinearColor::White);
            Stat.HoveredBorder->SetBrushColor(FLinearColor(0.2f, 0.2f, 0.2f, 1));
        }

        if (bHasEquipped)
        {
            int32 EquippedValue = 0;
            if (Stat.Label == "Health") EquippedValue = EquippedItem->ItemData.Health;
            else if (Stat.Label == "Armor") EquippedValue = EquippedItem->ItemData.Armor;
            else if (Stat.Label == "Dexterity") EquippedValue = EquippedItem->ItemData.Dexterity;
            else if (Stat.Label == "Strength") EquippedValue = EquippedItem->ItemData.Strength;
            else if (Stat.Label == "Intelligence") EquippedValue = EquippedItem->ItemData.Intelligence;
            else if (Stat.Label == "Luck") EquippedValue = EquippedItem->ItemData.Luck;
            else if (Stat.Label == "Slots") EquippedValue = EquippedItem->ItemData.Slots;

            const int32 Diff = Stat.Value - EquippedValue;

            // Equipped display
            if (EquippedValue == 0)
            {
                Stat.EquippedBox->SetVisibility(ESlateVisibility::Collapsed);
                Stat.EquippedBorder->SetVisibility(ESlateVisibility::Collapsed);
            }
            else
            {
                Stat.EquippedBox->SetVisibility(ESlateVisibility::Visible);
                Stat.EquippedBorder->SetVisibility(ESlateVisibility::Visible);

                // Display equipped stat with label, e.g. "+5 Slots"
                Stat.EquippedText->SetText(FText::FromString("+" + FString::FromInt(EquippedValue) + " " + Stat.Label));
                Stat.EquippedText->SetColorAndOpacity(FLinearColor::White);
                Stat.EquippedBorder->SetBrushColor(FLinearColor(0.2f, 0.2f, 0.2f, 1));
            }

            // Comparison display
            if (Diff != 0)
            {
                Stat.ComparisonBox->SetVisibility(ESlateVisibility::Visible);
                const bool bPositive = Diff > 0;
                FLinearColor DiffColor = bPositive ? FLinearColor::Green : FLinearColor::Red;
                Stat.ComparisonText->SetText(FText::FromString((bPositive ? "+" : "") + FString::FromInt(Diff) + " " + Stat.Label));
                Stat.ComparisonText->SetColorAndOpacity(DiffColor);
            }
            else
            {
                Stat.ComparisonBox->SetVisibility(ESlateVisibility::Collapsed);
                Stat.ComparisonBorder->SetVisibility(ESlateVisibility::Collapsed);
            }
        }
        else
        {
            // No equipped item: collapse both equipped + comparison columns
            Stat.EquippedBox->SetVisibility(ESlateVisibility::Collapsed);
            Stat.ComparisonBox->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}
