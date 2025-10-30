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

#include "InventoryItemOptions.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Inventory.h"
#include <Blueprint/WidgetLayoutLibrary.h>

#include "Blueprint/WidgetBlueprintLibrary.h"      
#include "RelicRunners/Vendor/UIVendor.h"          

UInventoryItemOptions* UInventoryItemOptions::CurrentPopup = nullptr;

void UInventoryItemOptions::NativeConstruct()
{
    Super::NativeConstruct();

    if (EquipButton)
    {
        EquipButton->OnClicked.AddDynamic(this, &UInventoryItemOptions::OnEquipClicked);
    }
    if (UnequipButton)
    {
        UnequipButton->OnClicked.AddDynamic(this, &UInventoryItemOptions::OnUnequipClicked);
    }
    if (DropButton)
    {
        DropButton->OnClicked.AddDynamic(this, &UInventoryItemOptions::OnDropClicked);
    }

    if (BuyButton)
    {
        BuyButton->OnClicked.AddDynamic(this, &UInventoryItemOptions::OnBuyClicked_Buy);
    }
    if (SellButton)
    {
        SellButton->OnClicked.AddDynamic(this, &UInventoryItemOptions::OnSellClicked_Sell);
    }
}

void UInventoryItemOptions::Setup(UItemObject* InItem)
{
    InitialScreenPosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
    Item = InItem;
    CurrentPopup = this;
}

void UInventoryItemOptions::NativeDestruct()
{
    Super::NativeDestruct();
    if (CurrentPopup == this)
    {
        CurrentPopup = nullptr;
    }
}

void UInventoryItemOptions::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    // Close if mouse moved too far
    FVector2D CurrentMousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
    float Distance = FVector2D::Distance(CurrentMousePos, InitialScreenPosition);

    if (Distance > 100.f) // Adjust as needed
    {
        ClosePopup();
        return;
    }

    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        if (PC->WasInputKeyJustPressed(EKeys::LeftMouseButton) ||
            PC->WasInputKeyJustPressed(EKeys::RightMouseButton))
        {
            ClosePopup();
        }
    }
}

void UInventoryItemOptions::ConfigureButtons(bool bShowEquip, bool bShowUnequip)
{
    if (EquipButton)
    {
        EquipButton->SetVisibility(bShowEquip ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }

    if (UnequipButton)
    {
        UnequipButton->SetVisibility(bShowUnequip ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
}

void UInventoryItemOptions::ClosePopup()
{
    if (UInventoryItemOptions::CurrentPopup == this)
    {
        UInventoryItemOptions::CurrentPopup = nullptr;
    }

    RemoveFromParent();
}

void UInventoryItemOptions::CloseAnyOpenPopup()
{
    if (CurrentPopup && CurrentPopup->IsInViewport())
    {
        CurrentPopup->RemoveFromParent();
        CurrentPopup = nullptr;
    }
}

void UInventoryItemOptions::OnEquipClicked()
{
    if (!Item) return;

    if (UInventory* inventory = UInventory::Get(this))
    {
        inventory->EquipItem(Item);
        RemoveFromParent();
    }
}

void UInventoryItemOptions::OnUnequipClicked()
{
    if (!Item) return;

    if (UInventory* Inventory = UInventory::Get(this))
    {
        Inventory->UnequipItem(Item);
        RemoveFromParent();
    }
}

void UInventoryItemOptions::OnDropClicked()
{
    if (!Item) return;

    if (UInventory* inventory = UInventory::Get(this))
    {
        inventory->DropItem(Item);
        RemoveFromParent();
    }
}

static UIVendor* FindVisibleVendorWidget(UUserWidget* Context)
{
    if (!Context) return nullptr;

    TArray<UUserWidget*> Widgets;
    UWidgetBlueprintLibrary::GetAllWidgetsOfClass(Context, Widgets, UIVendor::StaticClass(), /*TopLevelOnly*/ false);
    for (UUserWidget* W : Widgets)
    {
        if (W && W->IsInViewport())
        {
            const ESlateVisibility Vis = W->GetVisibility();
            if (Vis != ESlateVisibility::Hidden && Vis != ESlateVisibility::Collapsed)
            {
                return Cast<UIVendor>(W);
            }
        }
    }
    return nullptr;
}

void UInventoryItemOptions::OnBuyClicked_Buy()
{
    UIVendor* Vendor = VendorWidgetRef ? VendorWidgetRef : FindVisibleVendorWidget(this);
    if (Vendor && Item)
    {
        UE_LOG(LogTemp, Verbose, TEXT("[InventoryItemOptions] BUY %s"), *Item->GetUniqueID().ToString());
        Vendor->BuyByGuid(Item->GetUniqueID());   
    }
    
    RemoveFromParent();
}

void UInventoryItemOptions::OnSellClicked_Sell()
{
    UIVendor* Vendor = VendorWidgetRef ? VendorWidgetRef : FindVisibleVendorWidget(this);
    if (Vendor && Item)
    {
        UE_LOG(LogTemp, Verbose, TEXT("[InventoryItemOptions] SELL %s"), *Item->GetUniqueID().ToString());
        Vendor->SellByGuid(Item->GetUniqueID());  
    }
    
    RemoveFromParent();
}
