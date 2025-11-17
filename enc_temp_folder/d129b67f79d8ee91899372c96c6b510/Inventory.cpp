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

#include "Inventory.h"
#include "RelicRunners/Vendor/Vendor.h"
#include "Components/TileView.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "InventoryToolTip.h"
#include "InventorySortingOptions.h"
#include "InventoryComponent.h"
#include "Styling/SlateTypes.h"
#include "SlateBasics.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "RelicRunners/RelicRunnersCharacter.h"
#include "InventoryItemOptions.h"
#include "RelicRunners/Item/ItemActor.h"
#include "RelicRunners/Item/ItemStats.h"
#include "RelicRunners/Item/ItemData.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/DragDropOperation.h"
#include "RelicRunners/PlayerPreview/PlayerPreview.h"
#include "RelicRunners/PlayerState/RelicRunnersPlayerState.h"
#include <RelicRunners/Game/RelicRunnersGameInstance.h>

void UInventory::NativeConstruct()
{
    Super::NativeConstruct();

    TB_InventorySlots->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), 0, 0)));

    InventoryItemOptionsClass = LoadClass<UInventoryItemOptions>(nullptr, TEXT("/Game/Inventory/BP_InventoryItemOptions.BP_InventoryItemOptions_C"));
    InventorySortingOptionsClass = LoadClass<UInventorySortingOptions>(nullptr, TEXT("/Game/Inventory/BP_InventorySortingOptions.BP_InventorySortingOptions_C"));

    if (B_Helmet) B_Helmet->OnClicked.AddDynamic(this, &UInventory::HelmetSlotClicked);
    if (B_Arms) B_Arms->OnClicked.AddDynamic(this, &UInventory::ArmsSlotClicked);
    if (B_Upper) B_Upper->OnClicked.AddDynamic(this, &UInventory::UpperSlotClicked);
    if (B_Lower) B_Lower->OnClicked.AddDynamic(this, &UInventory::LowerSlotClicked);
    if (B_Mainhand) B_Mainhand->OnClicked.AddDynamic(this, &UInventory::MainhandSlotClicked);
    if (B_Offhand) B_Offhand->OnClicked.AddDynamic(this, &UInventory::OffhandlotClicked);
    if (B_Necklace) B_Necklace->OnClicked.AddDynamic(this, &UInventory::NecklaceSlotClicked);
    if (B_SortingType) B_SortingType->OnClicked.AddDynamic(this, &UInventory::SortingTypeClicked);
    
    //Vendor
    if (VendorCanvas) VendorCanvas->SetVisibility(ESlateVisibility::Collapsed);
    if (VendorSlots) VendorSlots->ClearListItems();
}

void UInventory::ToggleVendorUI(bool value)
{
    if (!VendorCanvas) return;

    if (!value)
    {
        VendorCanvas->SetVisibility(ESlateVisibility::Collapsed);
        VendorSlots->ClearListItems();
        CurrentVendor = nullptr;
    }
    else
    {
        VendorCanvas->SetVisibility(ESlateVisibility::Visible);
    }
}

void UInventory::DisplaySelectedVendorItems(const TArray<FItemData>& Items, AVendor* Vendor)
{
    VendorSlots->ClearListItems();
    CurrentVendor = Vendor;

    for (const FItemData& Data : Items)
    {
        UItemObject* NewItem = NewObject<UItemObject>(this);

        NewItem->ItemData = Data;

        VendorSlots->AddItem(NewItem);
    }
}

void UInventory::HandleSortSelected(EInventorySorting Method)
{
    if (TB_SortingType)
    {
        TB_SortingType->SetText(FText::FromString("Sort by : " + UInventorySortingOptions::GetSortingMethodDisplayName(Method)));
    }

    if (InventorySortingOptions)
    {
        InventorySortingOptions->RemoveFromParent();
        InventorySortingOptions = nullptr;
    }

    if (InventoryComponent->GetCurrentSortingMethod() != Method)
    {
        InventoryComponent->SetSortingMethod(Method);
    }

    RefreshInventoryUI();
}

void UInventory::RefreshInventoryUI()
{
    if (!InventorySlots || !InventoryComponent) return;

    InventorySlots->ClearListItems();

    for (UItemObject* Item : InventoryComponent->InventoryItems)
    {
        InventorySlots->AddItem(Item); 
    }

    OnEquippedStatsUpdated(InventoryComponent->CachedEquippedStats);
}

void UInventory::RefreshEquippedUI()
{
    TArray<FString> AllItemTypes = {
        "Helmet", "Upper", "Lower", "Arms",
        "Necklace", "Shield", "Sword"
    };

    for (const FString& Type : AllItemTypes)
    {
        UpdateButtonWithTooltip(nullptr, Type, nullptr, FLinearColor::Gray);
    }

    for (const FEquippedItemEntry& Pair : InventoryComponent->EquippedItems)
    {
        UItemObject* Item = Pair.Item;
        const FString& ItemType = Pair.ItemType;

        if (!Item) continue;

        if (TooltipWidgetClass)
        {
            if (UInventoryToolTip* ToolTip = CreateWidget<UInventoryToolTip>(GetWorld(), TooltipWidgetClass))
            {
                FLinearColor RarityColor = ItemStats::GetRarityDataMap()[Item->GetRarity()].Color;

                // Avoid comparing the item to itself
                UItemObject* EquippedItem = InventoryComponent->GetEquippedItemByType(ItemType);
                if (EquippedItem == Item)
                {
                    EquippedItem = nullptr;
                }

                ToolTip->Setup(Item, RarityColor, EquippedItem);
                UpdateButtonWithTooltip(Item, ItemType, ToolTip, RarityColor);
            }
        }
    }

    OnEquippedStatsUpdated(InventoryComponent->CachedEquippedStats);
}

UInventory::FSlotUIComponents UInventory::GetUIComponentsForItemType(const FString& ItemType)
{
    if (ItemType == "Helmet") return { B_Helmet, I_Helmet, TB_LevelHelmet };
    if (ItemType == "Arms") return { B_Arms, I_Arms, TB_LevelArms };
    if (ItemType == "Upper") return { B_Upper, I_Upper, TB_LevelUpper };
    if (ItemType == "Lower") return { B_Lower, I_Lower, TB_LevelLower };
    if (ItemType == "Necklace") return { B_Necklace, I_Necklace, TB_LevelNecklace };
    if (ItemType == "Shield") return { B_Offhand, I_Offhand, TB_LevelOffhand };
    if (ItemType == "Sword") return { B_Mainhand, I_Mainhand, TB_LevelMainhand };

    return { nullptr, nullptr, nullptr };
}

void UInventory::UpdateButtonWithTooltip(UItemObject* Item, const FString& itemType, UInventoryToolTip* ToolTip, const FLinearColor& Color)
{
    const FString& ItemType = itemType;

    auto Components = GetUIComponentsForItemType(itemType);
    if (!Components.Button || !Components.Image || !Components.LevelText) return;

    Components.Button->SetToolTip(ToolTip);

    const bool bHasEquipped = (ToolTip != nullptr);
    const float Opacity = bHasEquipped ? 1.0f : 0.3f;
    const FLinearColor Tint = bHasEquipped ? Color : FLinearColor(0.3f, 0.3f, 0.3f, Opacity);

    FButtonStyle Style = Components.Button->GetStyle();
    Style.Normal.TintColor = FSlateColor(Tint);
    Style.Hovered.TintColor = FSlateColor(Tint);
    Style.Pressed.TintColor = FSlateColor(Tint);
    Components.Button->SetStyle(Style);
    Components.Button->SetColorAndOpacity(Tint);

    Components.Image->SetColorAndOpacity(FLinearColor(1, 1, 1, Opacity));
    Components.LevelText->SetVisibility(ESlateVisibility::Visible);

    if (Item)
    {
        Components.LevelText->SetText(FText::AsNumber(Item->GetLevel()));
    }
    else
    {
        Components.LevelText->SetText(FText::FromString(""));
    }
}

void UInventory::SetInventoryComponent(UInventoryComponent* NewComponent)
{
    if (!NewComponent || !NewComponent->IsRegistered() || !NewComponent->GetOwner())
    {
        // Try again next frame if it's not ready
        UE_LOG(LogTemp, Warning, TEXT("[UI] InventoryComponent not ready, retrying..."));

        FTimerHandle RetryHandle;
        GetWorld()->GetTimerManager().SetTimer(RetryHandle, [this, NewComponent]() { SetInventoryComponent(NewComponent); }, 0.1f, false);

        return;
    }

    InventoryComponent = NewComponent;

    // Prevent double binding
    InventoryComponent->OnInventoryChanged.RemoveDynamic(this, &UInventory::RefreshInventoryUI);
    InventoryComponent->OnEquipmentChanged.RemoveDynamic(this, &UInventory::RefreshEquippedUI);
    InventoryComponent->OnStatsChanged.RemoveDynamic(this, &UInventory::OnEquippedStatsUpdated);
    // Add the new binds
    InventoryComponent->OnInventoryChanged.AddDynamic(this, &UInventory::RefreshInventoryUI);
    InventoryComponent->OnEquipmentChanged.AddDynamic(this, &UInventory::RefreshEquippedUI);
    InventoryComponent->OnStatsChanged.AddDynamic(this, &UInventory::OnEquippedStatsUpdated);

    UE_LOG(LogTemp, Log, TEXT("[UI] InventoryComponent successfully bound to delegates."));

    APlayerController* PC = GetOwningPlayer();
    PC = GetOwningPlayer();
    if (PC)
    {
        APawn* Pawn = PC->GetPawn();
        OwningCharacter = Cast<ARelicRunnersCharacter>(Pawn);
        if (OwningCharacter)
        {
            LocalInventory = OwningCharacter->GetInventoryComponent();
        }
    }
    
    InventoryComponent->SetSortingMethod(EInventorySorting::SortByRarity);
    OnEquippedStatsUpdated(InventoryComponent->GetCachedEquippedStats());
}

void UInventory::SetPreviewActorImage(UTextureRenderTarget2D* RenderTarget)
{
    UE_LOG(LogTemp, Warning, TEXT("[UI] Trying to set preview image with RenderTarget: %s"), *GetNameSafe(RenderTarget));

    if (!RenderTarget || !I_PlayerPreview) return;

    UMaterialInterface* BaseMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/ThirdPerson/Blueprints/PlayerPreview/M_PlayerPreview.M_PlayerPreview"));

    if (!BaseMaterial)
    {
        UE_LOG(LogTemp, Error, TEXT("[UI] Failed to load base material."));
        return;
    }

    // Create the dynamic material
    UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
    if (!DynMaterial)
    {
        UE_LOG(LogTemp, Error, TEXT("[UI] Failed to create dynamic material."));
        return;
    }

    DynMaterial->SetTextureParameterValue(FName("PreviewTexture"), RenderTarget);
    I_PlayerPreview->SetBrushFromMaterial(DynMaterial);

    UE_LOG(LogTemp, Warning, TEXT("[UI] Set preview image with RenderTarget: %s"), *GetNameSafe(RenderTarget));
}

UInventory* UInventory::Get(UObject* Context)
{
    if (!Context) return nullptr;

    APlayerController* PC = UGameplayStatics::GetPlayerController(Context, 0);
    if (!PC) return nullptr;

    ARelicRunnersCharacter* Character = Cast<ARelicRunnersCharacter>(PC->GetPawn());
    if (!Character) return nullptr;

    return Character->GetInventory();
}

void UInventory::EquipItem(UItemObject* Item)
{
    if (OwningCharacter && Item)
    {
        OwningCharacter->Server_EquipItemByID(Item->ItemData.UniqueID);
    }
}

void UInventory::UnequipItem(UItemObject* Item)
{
    if (OwningCharacter && Item)
    {
        OwningCharacter->Server_UnequipItemByID(Item->ItemData.UniqueID);
    }
}

void UInventory::DropItem(UItemObject* Item)
{
    if (!Item) return;

    if (InventoryComponent->GetEquippedItemReference(Item->ItemData.ItemType) == Item)
    {
        UnequipItem(Item);
    }
    InventorySlots->RemoveItem(Item);

    OwningCharacter->Server_DropItem(Item->ItemData);
}

void UInventory::BuyItem(UItemObject* Item)
{
    if (!Item) return;

    if (OwningCharacter)
    {
        OwningCharacter->Server_BuyItem(Item->ItemData, CurrentVendor);
    }
}

void UInventory::SellItem(UItemObject* Item)
{
    if (!Item) return;

    if (OwningCharacter)
    {
        OwningCharacter->Server_SellItem(Item->ItemData, CurrentVendor);
    }
}

FReply UInventory::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    UInventoryItemOptions::CloseAnyOpenPopup();
    UInventorySortingOptions::CloseAnyOpenPopup();
    return FReply::Unhandled(); 
}

void UInventory::HelmetSlotClicked()
{
    OnEquippedSlotClick(InventoryComponent->GetEquippedItemByType("Helmet"));
}

void UInventory::ArmsSlotClicked()
{
    OnEquippedSlotClick(InventoryComponent->GetEquippedItemByType("Arms"));
}

void UInventory::UpperSlotClicked()
{
    OnEquippedSlotClick(InventoryComponent->GetEquippedItemByType("Upper"));
}

void UInventory::LowerSlotClicked()
{
    OnEquippedSlotClick(InventoryComponent->GetEquippedItemByType("Lower"));
}

void UInventory::NecklaceSlotClicked()
{
    OnEquippedSlotClick(InventoryComponent->GetEquippedItemByType("Necklace"));
}

void UInventory::MainhandSlotClicked()
{
    OnEquippedSlotClick(InventoryComponent->GetEquippedItemByType("Sword"));
}

void UInventory::OffhandlotClicked()
{
    OnEquippedSlotClick(InventoryComponent->GetEquippedItemByType("Shield"));
}

void UInventory::SortingTypeClicked()
{
    if (InventorySortingOptions && InventorySortingOptions->IsInViewport())
    {
        InventorySortingOptions->RemoveFromParent();
        InventorySortingOptions = nullptr;
        return;
    }

    if (InventorySortingOptionsClass)
    {
        InventorySortingOptions = CreateWidget<UInventorySortingOptions>(GetWorld(), InventorySortingOptionsClass);
        if (InventorySortingOptions)
        {
            InventorySortingOptions->OnSortingSelected.AddDynamic(this, &UInventory::HandleSortSelected);

            InventorySortingOptions->Setup();
            FVector2D ScreenPosition;
            UWidgetLayoutLibrary::GetMousePositionScaledByDPI(GetWorld()->GetFirstPlayerController(), ScreenPosition.X, ScreenPosition.Y);
            InventorySortingOptions->AddToViewport();
            InventorySortingOptions->SetPositionInViewport(ScreenPosition + FVector2D(0, 30.f), false);
        }
    }
}

void UInventory::OnEquippedSlotClick(UItemObject* EquippedItem)
{
    UInventorySortingOptions::CloseAnyOpenPopup();
    UInventoryItemOptions::CloseAnyOpenPopup();
    if (EquippedItem)
    {
        UInventoryItemOptions* Popup = CreateWidget<UInventoryItemOptions>(GetWorld(), InventoryItemOptionsClass);
        if (Popup)
        {
            Popup->AddToViewport();
            Popup->Setup(EquippedItem);

            // (Hide Equip, Show Unequip, Show Sell, Show Buy) for equipped items
            Popup->ConfigureButtons(false, true, false, false);

            FVector2D ScreenPosition;
            UWidgetLayoutLibrary::GetMousePositionScaledByDPI(GetWorld()->GetFirstPlayerController(), ScreenPosition.X, ScreenPosition.Y);
            Popup->SetPositionInViewport(ScreenPosition, false);
        }
    }
}

bool UInventory::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

    if (!InOperation)
        return false;

    UItemObject* DraggedItem = Cast<UItemObject>(InOperation->Payload);
    if (!DraggedItem) return false;

    FVector2D DropPosition = InGeometry.AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition());

    //Equipped Items Location
    FGeometry EquippedGeometry = EquippedCanvas->GetCachedGeometry();
    const FVector2D EquippedLocal = EquippedGeometry.AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition());
    const FVector2D EquippedSize = EquippedGeometry.GetLocalSize();
    const bool bInsideEquipped = EquippedLocal.X >= 0.f && EquippedLocal.Y >= 0.f && EquippedLocal.X <= EquippedSize.X && EquippedLocal.Y <= EquippedSize.Y;

    //Inventory Items Location
    FGeometry InventoryGeometry = InventorySlots->GetCachedGeometry();
    const FVector2D InventoryLocal = InventoryGeometry.AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition());
    const FVector2D InventorySize = InventoryGeometry.GetLocalSize();
    const bool bInsideInventory = InventoryLocal.X >= 0.f && InventoryLocal.Y >= 0.f && InventoryLocal.X <= InventoryLocal.X && InventoryLocal.Y <= InventoryLocal.Y;

    //Vendor Forge Location
    FGeometry ForgeGeometry = ForgeCanvas->GetCachedGeometry();
    const FVector2D ForgeLocal = ForgeGeometry.AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition());
    const FVector2D ForgeSize = ForgeGeometry.GetLocalSize();
    const bool bInsideForge = ForgeLocal.X >= 0.f && ForgeLocal.Y >= 0.f && ForgeLocal.X <= ForgeLocal.X && ForgeLocal.Y <= ForgeLocal.Y;

    //Vendor Shop Location
    FGeometry ShopGeometry = ShopCanvas->GetCachedGeometry();
    const FVector2D ShopLocal = ShopGeometry.AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition());
    const FVector2D ShopSize = ShopGeometry.GetLocalSize();
    const bool bInsideShop = ShopLocal.X >= 0.f && ShopLocal.Y >= 0.f && ShopLocal.X <= ShopLocal.X && ShopLocal.Y <= ShopLocal.Y;

    if (bInsideEquipped)
    {
        EquipItem(DraggedItem);
    }
    else if (bInsideForge && VendorCanvas->IsVisible())
    {
        FuseBorder->SetVisibility(ESlateVisibility::Visible);
        TB_Odds->SetVisibility(ESlateVisibility::Visible);
        TB_Odds1->SetVisibility(ESlateVisibility::Visible);
        TB_Odds2->SetVisibility(ESlateVisibility::Visible);
    }
    else if (bInsideShop && VendorCanvas->IsVisible())
    {
        SellItem(DraggedItem);
    }
    else if (bInsideInventory)
    {
        return true;
    }
    else
    {
        DropItem(DraggedItem);
    }

    return true;
}

void UInventory::OnEquippedStatsUpdated(const FEquippedStatsSummary& Stats)
{
    if (TB_Health)       TB_Health->SetText(FText::AsNumber(Stats.TotalHealth));
    if (TB_Armor)        TB_Armor->SetText(FText::AsNumber(Stats.TotalArmor));
    if (TB_Dexterity)    TB_Dexterity->SetText(FText::AsNumber(Stats.TotalDexterity));
    if (TB_Strength)     TB_Strength->SetText(FText::AsNumber(Stats.TotalStrength));
    if (TB_Intelligence) TB_Intelligence->SetText(FText::AsNumber(Stats.TotalIntelligence));
    if (TB_Luck)         TB_Luck->SetText(FText::AsNumber(Stats.TotalLuck));

    if (OwningCharacter)
    {
        if (TB_Gold) TB_Gold->SetText(FText::AsNumber(OwningCharacter->GetPlayerGold()));
        if (ARelicRunnersPlayerState* PS = Cast<ARelicRunnersPlayerState>(OwningCharacter->GetPlayerState()))
        {
            if(TB_Username) TB_Username->SetText(FText::FromString(PS->GetPlayerName()));
        }
    }

    int NumItems = InventoryComponent->InventoryItems.Num();
    int MaxSlots = Stats.TotalSlots;

    TB_InventorySlots->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), NumItems, MaxSlots)));
}