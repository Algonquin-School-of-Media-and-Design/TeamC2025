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
#include "Components/TextBlock.h"
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
#include <Blueprint/WidgetBlueprintLibrary.h>

void UInventorySlotsEntry::NativeConstruct()
{
    Super::NativeConstruct();

    InventoryItemOptionsClass = LoadClass<UInventoryItemOptions>(nullptr, TEXT("/Game/Inventory/BP_InventoryItemOptions.BP_InventoryItemOptions_C"));
}

void UInventorySlotsEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    Item = Cast<UItemObject>(ListItemObject);
    if (!Item) return;

    TB_Level->SetText(FText::FromString(FString::FromInt(Item->GetLevel())));
    TB_Level->SetColorAndOpacity(FLinearColor(0.7f, 0.7f, 0.7f, 1));

    // Set border color by rarity
    FLinearColor ItemFrameColor = ItemStats::GetRarityDataMap()[Item->GetRarity()].Color;
    B_Item->SetBrushColor(ItemFrameColor);
    B_Color->SetBrushColor(ItemFrameColor);

    // Load and assign icon texture
    FString ImagePath = FString::Printf(TEXT("Texture2D'/Game/ThirdPerson/Icons/%s.%s'"),
        *Item->GetItemType(), *Item->GetItemType());
    if (UTexture2D* Texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *ImagePath)))
    {
        I_Item->SetBrushFromTexture(Texture);
    }
}

FReply UInventorySlotsEntry::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
    {
        bIsPressed = true;
        bIsDragging = false;
        PressedTime = GetWorld()->GetRealTimeSeconds();

        FEventReply EventReply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
        return EventReply.NativeReply;
    }

    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UInventorySlotsEntry::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (!bIsPressed) return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);

    bIsPressed = false;

    float HeldDuration = GetWorld()->GetRealTimeSeconds() - PressedTime;

    if (!bIsDragging && HeldDuration < 0.25f)
    {
        OnEntryClicked();
    }

    bIsDragging = false;
    return FReply::Handled();
}

void UInventorySlotsEntry::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
    if (!Item) return;

    bIsDragging = true;

    UDragDropOperation* DragOp = UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass());
    DragOp->Payload = Item;
    DragOp->DefaultDragVisual = this;
    DragOp->Pivot = EDragPivot::MouseDown;
    OutOperation = DragOp;
}

void UInventorySlotsEntry::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

    if (!Item || !TooltipWidgetClass || !B_Item) return;

    UInventoryToolTip* ToolTip = CreateWidget<UInventoryToolTip>(GetWorld(), TooltipWidgetClass);
    if (!ToolTip) return;

    FLinearColor RarityColor = ItemStats::GetRarityDataMap()[Item->GetRarity()].Color;

    // Find equipped item of same type
    ARelicRunnersPlayerController* PC = Cast<ARelicRunnersPlayerController>(GetWorld()->GetFirstPlayerController());
    UItemObject* EquippedItem = nullptr;
    if (PC && PC->GetPossessedPawn())
    {
        if (UInventoryComponent* InventoryComponent = PC->GetPossessedPawn()->GetInventoryComponent())
        {
            EquippedItem = InventoryComponent->GetEquippedItemByType(Item->GetItemType());
            if (EquippedItem == Item)
                EquippedItem = nullptr;
        }
    }

    ToolTip->Setup(Item, RarityColor, EquippedItem);
    B_Item->SetToolTip(ToolTip);
}

void UInventorySlotsEntry::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);
    B_Item->SetToolTip(nullptr);
}

void UInventorySlotsEntry::OnEntryClicked()
{
    UInventoryItemOptions::CloseAnyOpenPopup();
    UInventorySortingOptions::CloseAnyOpenPopup();

    if (!InventoryItemOptionsClass || !Item) return;

    UInventoryItemOptions* Popup = CreateWidget<UInventoryItemOptions>(GetWorld(), InventoryItemOptionsClass);
    if (Popup)
    {
        Popup->AddToViewport();
        Popup->Setup(Item);
        Popup->ConfigureButtons(true, false);

        FVector2D ScreenPosition;
        UWidgetLayoutLibrary::GetMousePositionScaledByDPI(GetWorld()->GetFirstPlayerController(), ScreenPosition.X, ScreenPosition.Y);
        Popup->SetPositionInViewport(ScreenPosition, false);
    }
}