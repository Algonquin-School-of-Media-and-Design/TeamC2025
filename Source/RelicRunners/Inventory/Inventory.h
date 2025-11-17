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
#include "Inventory.generated.h"

enum class EInventorySorting : uint8;

UCLASS()
class RELICRUNNERS_API UInventory : public UUserWidget
{
	GENERATED_BODY()
public:

	//Structs
	struct FSlotUIComponents
	{
		class UButton* Button;
		class UImage* Image;
		class UTextBlock* LevelText;
	};

	//Properties
	UPROPERTY()
	class UInventoryComponent* InventoryComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tooltip")
	TSubclassOf<UUserWidget> TooltipWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Popup")
	TSubclassOf<class UInventoryItemOptions> InventoryItemOptionsClass;
	UPROPERTY(EditAnywhere, Category = "Popup")
	TSubclassOf<class UInventorySortingOptions> InventorySortingOptionsClass;
	UPROPERTY(EditAnywhere, Category = "Popup")
	class UInventorySortingOptions* InventorySortingOptions = nullptr;
	UPROPERTY()
	class ARelicRunnersCharacter* OwningCharacter;
	UPROPERTY()
	class UInventoryComponent* LocalInventory;

	class AVendor* CurrentVendor;

	//Functions
	virtual void NativeConstruct() override;
	void ToggleVendorUI(bool value);
	void DisplaySelectedVendorItems(const TArray<struct FItemData>& Items, AVendor* Vendor);
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	FSlotUIComponents GetUIComponentsForItemType(const FString& ItemType);
	void UpdateButtonWithTooltip(class UItemObject* Item, const FString& itemType, class UInventoryToolTip* ToolTip, const FLinearColor& Color);
	void SetInventoryComponent(UInventoryComponent* NewComponent);
	void SetPreviewActorImage(class UTextureRenderTarget2D* RenderTarget);
	static UInventory* Get(UObject* Context);
	void UnequipItem(class UItemObject* Item);
	void EquipItem(class UItemObject* Item);
	void DropItem(class UItemObject* Item);
	void BuyItem(class UItemObject* Item);
	void SellItem(class UItemObject* Item);
	void OnEquippedSlotClick(class UItemObject* EquippedItem);
	UFUNCTION()
	void RefreshInventoryUI();
	UFUNCTION()
	void RefreshEquippedUI();
	UFUNCTION()
	void HandleSortSelected(EInventorySorting Method);
	UFUNCTION()
	void OnEquippedStatsUpdated(const struct FEquippedStatsSummary& Stats);
	UFUNCTION()
	void HelmetSlotClicked();
	UFUNCTION()
	void ArmsSlotClicked();
	UFUNCTION()
	void UpperSlotClicked();
	UFUNCTION()
	void LowerSlotClicked();
	UFUNCTION()
	void NecklaceSlotClicked();
	UFUNCTION()
	void MainhandSlotClicked();
	UFUNCTION()
	void OffhandlotClicked();
	UFUNCTION()
	void SortingTypeClicked();


	//UI Elements
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTileView* InventorySlots;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTileView* VendorSlots;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* VendorCanvas;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* ForgeCanvas;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* ShopCanvas;

	UPROPERTY(meta = (BindWidget))
	class UBorder* FuseBorder;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_Odds;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_Odds1;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_Odds2;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_InventorySlots;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_Username;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_Health;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_Armor;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_Dexterity;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_Strength;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_Intelligence;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_Luck;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_Gold;
	UPROPERTY(meta = (BindWidget))
	class UButton* B_Helmet;
	UPROPERTY(meta = (BindWidget))
	class UButton* B_Upper;
	UPROPERTY(meta = (BindWidget))
	class UButton* B_Lower;
	UPROPERTY(meta = (BindWidget))
	class UButton* B_Necklace;
	UPROPERTY(meta = (BindWidget))
	class UButton* B_Arms;
	UPROPERTY(meta = (BindWidget))
	class UButton* B_Mainhand;
	UPROPERTY(meta = (BindWidget))
	class UButton* B_Offhand;
	UPROPERTY(meta = (BindWidget))
	class UButton* B_SortingType;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_SortingType;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_LevelHelmet;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_LevelUpper;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_LevelLower;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_LevelNecklace;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_LevelArms;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_LevelMainhand;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_LevelOffhand;
	UPROPERTY(meta = (BindWidget))
	class UImage* I_PlayerPreview;
	UPROPERTY(meta = (BindWidget))
	class UImage* I_Helmet;
	UPROPERTY(meta = (BindWidget))
	class UImage* I_Upper;
	UPROPERTY(meta = (BindWidget))
	class UImage* I_Lower;
	UPROPERTY(meta = (BindWidget))
	class UImage* I_Necklace;
	UPROPERTY(meta = (BindWidget))
	class UImage* I_Arms;
	UPROPERTY(meta = (BindWidget))
	class UImage* I_Mainhand;
	UPROPERTY(meta = (BindWidget))
	class UImage* I_Offhand;
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* EquippedCanvas;
	
private:

	
};
