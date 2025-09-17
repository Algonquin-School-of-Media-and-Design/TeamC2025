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
#include "Components/TileView.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/Image.h"
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
#include "RelicRunners/PlayerPreview/PlayerPreview.h"
#include "RelicRunners/PlayerState/RelicRunnersPlayerState.h"

void UInventory::NativeConstruct()
{
    Super::NativeConstruct();

    TB_InventorySlots->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), 0, 0)));

    InventoryItemOptionsClass = LoadClass<UInventoryItemOptions>(nullptr, TEXT("/Game/Inventory/BP_InventoryItemOptions.BP_InventoryItemOptions_C"));
    InventorySortingOptionsClass = LoadClass<UInventorySortingOptions>(nullptr, TEXT("/Game/Inventory/BP_InventorySortingOptions.BP_InventorySortingOptions_C"));

    B_Backpack->OnClicked.AddDynamic(this, &UInventory::BackpackSlotClicked);
    B_Helmet->OnClicked.AddDynamic(this, &UInventory::HelmetSlotClicked);
    B_Waist->OnClicked.AddDynamic(this, &UInventory::WaistSlotClicked);
    B_Shoulders->OnClicked.AddDynamic(this, &UInventory::ShouldersSlotClicked);
    B_Gloves->OnClicked.AddDynamic(this, &UInventory::GlovesSlotClicked);
    B_Wrist->OnClicked.AddDynamic(this, &UInventory::WristSlotClicked);
    B_Chestplate->OnClicked.AddDynamic(this, &UInventory::ChestplateSlotClicked);
    B_Boots->OnClicked.AddDynamic(this, &UInventory::BootsSlotClicked);
    B_Leggings->OnClicked.AddDynamic(this, &UInventory::LeggingsSlotClicked);
    B_Mainhand->OnClicked.AddDynamic(this, &UInventory::MainhandSlotClicked);
    B_Offhand->OnClicked.AddDynamic(this, &UInventory::OffhandlotClicked);
    B_Necklace->OnClicked.AddDynamic(this, &UInventory::NecklaceSlotClicked);
    B_SortingType->OnClicked.AddDynamic(this, &UInventory::SortingTypeClicked);
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
    // Step 1: Clear all slot visuals
    TArray<FString> AllItemTypes = {
        "Helmet", "Chestplate", "Shoulders", "Leggings", "Boots", "Gloves",
        "Wrist", "Waist", "Necklace", "Backpack", "Shield", "Sword"
    };

    for (const FString& Type : AllItemTypes)
    {
        UpdateButtonWithTooltip(nullptr, Type, nullptr, FLinearColor::Gray);
    }

    // Step 2: Apply visuals
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
    if (ItemType == "Chestplate") return { B_Chestplate, I_Chestplate, TB_LevelChestplate };
    if (ItemType == "Shoulders") return { B_Shoulders, I_Shoulders, TB_LevelShoulders };
    if (ItemType == "Leggings") return { B_Leggings, I_Leggings, TB_LevelLeggings };
    if (ItemType == "Boots") return { B_Boots, I_Boots, TB_LevelBoots };
    if (ItemType == "Gloves") return { B_Gloves, I_Gloves, TB_LevelGloves };
    if (ItemType == "Wrist") return { B_Wrist, I_Wrist, TB_LevelWrist };
    if (ItemType == "Waist") return { B_Waist, I_Waist, TB_LevelWaist };
    if (ItemType == "Necklace") return { B_Necklace, I_Necklace, TB_LevelNecklace };
    if (ItemType == "Backpack") return { B_Backpack, I_Backpack, TB_LevelBackpack };
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

        // Schedule retry
        FTimerHandle RetryHandle;
        GetWorld()->GetTimerManager().SetTimer(RetryHandle, [this, NewComponent]()
            {
                SetInventoryComponent(NewComponent); // retry
            }, 0.1f, false);

        return;
    }

    InventoryComponent = NewComponent;

    // Prevent double binding
    InventoryComponent->OnInventoryChanged.RemoveDynamic(this, &UInventory::RefreshInventoryUI);
    InventoryComponent->OnEquipmentChanged.RemoveDynamic(this, &UInventory::RefreshEquippedUI);
    InventoryComponent->OnStatsChanged.RemoveDynamic(this, &UInventory::OnEquippedStatsUpdated);

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

    UMaterialInterface* BaseMaterial = LoadObject<UMaterialInterface>(
        nullptr,
        TEXT("/Game/ThirdPerson/Blueprints/PlayerPreview/M_PlayerPreview.M_PlayerPreview")
    );

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

    //  Use this to apply the material directly to the UImage widget
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

    // Check if item is equipped
    if (InventoryComponent->GetEquippedItemReference(Item->ItemData.ItemType) == Item)
    {
        UnequipItem(Item);
    }

    InventorySlots->RemoveItem(Item);

    if (OwningCharacter->HasAuthority())
    {
        OwningCharacter->Server_DropItem(Item->ItemData);
    }
    else
    {
        OwningCharacter->Server_DropItem(Item->ItemData);
    }
}

FReply UInventory::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    UInventoryItemOptions::CloseAnyOpenPopup();
    return FReply::Unhandled(); 
}

void UInventory::BackpackSlotClicked()
{
    OnEquippedSlotClick(InventoryComponent->GetEquippedItemByType("Backpack"));
}

void UInventory::HelmetSlotClicked()
{
    OnEquippedSlotClick(InventoryComponent->GetEquippedItemByType("Helmet"));
}

void UInventory::WaistSlotClicked()
{
    OnEquippedSlotClick(InventoryComponent->GetEquippedItemByType("Waist"));
}

void UInventory::ShouldersSlotClicked()
{
    OnEquippedSlotClick(InventoryComponent->GetEquippedItemByType("Shoulders"));
}

void UInventory::GlovesSlotClicked()
{
    OnEquippedSlotClick(InventoryComponent->GetEquippedItemByType("Gloves"));
}

void UInventory::WristSlotClicked()
{
    OnEquippedSlotClick(InventoryComponent->GetEquippedItemByType("Wrist"));
}

void UInventory::ChestplateSlotClicked()
{
    OnEquippedSlotClick(InventoryComponent->GetEquippedItemByType("Chestplate"));
}

void UInventory::BootsSlotClicked()
{
    OnEquippedSlotClick(InventoryComponent->GetEquippedItemByType("Boots"));
}

void UInventory::LeggingsSlotClicked()
{
    OnEquippedSlotClick(InventoryComponent->GetEquippedItemByType("Leggings"));
}

void UInventory::MainhandSlotClicked()
{
    OnEquippedSlotClick(InventoryComponent->GetEquippedItemByType("Sword"));
}

void UInventory::OffhandlotClicked()
{
    OnEquippedSlotClick(InventoryComponent->GetEquippedItemByType("Shield"));
}

void UInventory::NecklaceSlotClicked()
{
    OnEquippedSlotClick(InventoryComponent->GetEquippedItemByType("Necklace"));
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

            FVector2D ScreenPosition;
            UWidgetLayoutLibrary::GetMousePositionScaledByDPI(GetWorld()->GetFirstPlayerController(), ScreenPosition.X, ScreenPosition.Y);
            InventorySortingOptions->AddToViewport();
            InventorySortingOptions->SetPositionInViewport(ScreenPosition + FVector2D(0, 30.f), false);
        }
    }
}

void UInventory::OnEquippedSlotClick(UItemObject* EquippedItem)
{
    UInventoryItemOptions::CloseAnyOpenPopup();
    if (EquippedItem)
    {
        UInventoryItemOptions* Popup = CreateWidget<UInventoryItemOptions>(GetWorld(), InventoryItemOptionsClass);
        if (Popup)
        {
            Popup->AddToViewport();
            Popup->Setup(EquippedItem);

            // Hide Equip, Show Unequip for equipped items
            Popup->ConfigureButtons(false, true);

            FVector2D ScreenPosition;
            UWidgetLayoutLibrary::GetMousePositionScaledByDPI(GetWorld()->GetFirstPlayerController(), ScreenPosition.X, ScreenPosition.Y);
            Popup->SetPositionInViewport(ScreenPosition, false);
        }
    }
}

void UInventory::OnEquippedStatsUpdated(const FEquippedStatsSummary& Stats)
{
    if (TB_Health)       TB_Health->SetText(FText::AsNumber(Stats.TotalHealth));
    if (TB_Armor)        TB_Armor->SetText(FText::AsNumber(Stats.TotalArmor));
    if (TB_Dexterity)    TB_Dexterity->SetText(FText::AsNumber(Stats.TotalDexterity));
    if (TB_Strength)     TB_Strength->SetText(FText::AsNumber(Stats.TotalStrength));
    if (TB_Intelligence) TB_Intelligence->SetText(FText::AsNumber(Stats.TotalIntelligence));
    if (TB_Luck)         TB_Luck->SetText(FText::AsNumber(Stats.TotalLuck));

    if (TB_Username && OwningCharacter)
    {
        if (ARelicRunnersPlayerState* PS = OwningCharacter->GetPlayerState<ARelicRunnersPlayerState>())
        {
            TB_Username->SetText(FText::FromString(PS->GetPlayerName()));
        }
    }

    int NumItems = InventoryComponent->InventoryItems.Num();
    int MaxSlots = Stats.TotalSlots;

    TB_InventorySlots->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), NumItems, MaxSlots)));
}