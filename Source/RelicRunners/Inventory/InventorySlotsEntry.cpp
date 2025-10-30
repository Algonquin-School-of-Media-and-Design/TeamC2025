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

#include "InventorySlotsEntry.h"
#include "Styling/SlateTypes.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/HorizontalBox.h"
#include "Components/CanvasPanelSlot.h"
#include "SlateBasics.h"
#include "InventoryToolTip.h"
#include "InventoryItemOptions.h"
#include "InventorySortingOptions.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "RelicRunners/Item/ItemStats.h"
#include "RelicRunners/Item/ItemData.h"
#include "Inventory.h"
#include <RelicRunners/PlayerController/RelicRunnersPlayerController.h>
#include "InventoryComponent.h"
#include <RelicRunners/RelicRunnersCharacter.h>

void UInventorySlotsEntry::NativeConstruct()
{
    Super::NativeConstruct();

    InventoryItemOptionsClass = LoadClass<UInventoryItemOptions>(nullptr, TEXT("/Game/Inventory/BP_InventoryItemOptions.BP_InventoryItemOptions_C"));

    if (B_ItemFrame)
    {
        B_ItemFrame->OnClicked.AddDynamic(this, &UInventorySlotsEntry::OnEntryButtonClicked);
    }
}

void UInventorySlotsEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    Item = Cast<UItemObject>(ListItemObject);
    if (Item)
    {
        TB_Level->SetText(FText::FromString(FString::FromInt(Item->GetLevel())));

        FLinearColor ItemFrameColor = ItemStats::GetRarityDataMap()[Item->GetRarity()].Color;

        FButtonStyle NewStyle = B_ItemFrame->GetStyle();
        NewStyle.Normal.TintColor = FSlateColor(ItemFrameColor);
        NewStyle.Hovered.TintColor = FSlateColor(ItemFrameColor);
        NewStyle.Pressed.TintColor = FSlateColor(ItemFrameColor);
        B_ItemFrame->SetStyle(NewStyle);

        TB_Level->SetColorAndOpacity(FLinearColor(0.7f, 0.7f, 0.7f, 1));

        FString imagePath = "Texture2D'/Game/ThirdPerson/Icons/" + Item->GetItemType() + "." + Item->GetItemType() + "'";
        UTexture2D* Texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *imagePath));
        I_Item->SetBrushFromTexture(Texture);
    }
}

void UInventorySlotsEntry::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

    if (!Item || !TooltipWidgetClass || !B_ItemFrame) return;

    // Create a new tooltip each time we hover (so it updates correctly)
    UInventoryToolTip* ToolTip = CreateWidget<UInventoryToolTip>(GetWorld(), TooltipWidgetClass);
    if (!ToolTip) return;

    FLinearColor RarityColor = ItemStats::GetRarityDataMap()[Item->GetRarity()].Color;

    // Try to find the equipped item of the same type
    ARelicRunnersPlayerController* PC = Cast<ARelicRunnersPlayerController>(GetWorld()->GetFirstPlayerController());
    UItemObject* EquippedItem = nullptr;

    if (PC && PC->GetPossessedPawn())
    {
        UInventoryComponent* InventoryComponent = PC->GetPossessedPawn()->GetInventoryComponent();
        if (InventoryComponent)
        {
            EquippedItem = InventoryComponent->GetEquippedItemByType(Item->GetItemType());

            // Don't compare to itself
            if (EquippedItem == Item)
            {
                EquippedItem = nullptr;
            }
        }
    }

    ToolTip->Setup(Item, RarityColor, EquippedItem);
    B_ItemFrame->SetToolTip(ToolTip);
}

void UInventorySlotsEntry::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);
    B_ItemFrame->SetToolTip(nullptr);
}

void UInventorySlotsEntry::OnEntryButtonClicked()
{
    UInventoryItemOptions::CloseAnyOpenPopup();
    UInventorySortingOptions::CloseAnyOpenPopup();

    if (InventoryItemOptionsClass && Item)
    {
        UInventoryItemOptions* Popup = CreateWidget<UInventoryItemOptions>(GetWorld(), InventoryItemOptionsClass);
        if (Popup)
        {
            Popup->AddToViewport();
            Popup->Setup(Item);
            Popup->ConfigureButtons(true, false); // Show Equip, Hide Unequip

            FVector2D ScreenPosition;
            UWidgetLayoutLibrary::GetMousePositionScaledByDPI(GetWorld()->GetFirstPlayerController(), ScreenPosition.X, ScreenPosition.Y);
            Popup->SetPositionInViewport(ScreenPosition, false);
        }
    }
}

