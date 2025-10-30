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

	UPROPERTY()
	class UInventoryComponent* InventoryComponent;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTileView* InventorySlots;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tooltip")
	TSubclassOf<UUserWidget> TooltipWidgetClass;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Popup")
	TSubclassOf<class UInventoryItemOptions> InventoryItemOptionsClass;

	UPROPERTY(EditAnywhere, Category = "Popup")
	TSubclassOf<class UInventorySortingOptions> InventorySortingOptionsClass;

	class UInventorySortingOptions* InventorySortingOptions = nullptr;

	struct FSlotUIComponents
	{
		class UButton* Button;
		class UImage* Image;
		class UTextBlock* LevelText;
	};

	FSlotUIComponents GetUIComponentsForItemType(const FString& ItemType);

	void UpdateButtonWithTooltip(class UItemObject* Item, const FString& itemType, class UInventoryToolTip* ToolTip, const FLinearColor& Color);

	void SetInventoryComponent(UInventoryComponent* NewComponent);

	void SetPreviewActorImage(class UTextureRenderTarget2D* RenderTarget);

	static UInventory* Get(UObject* Context);
	void UnequipItem(UItemObject* Item);
	void EquipItem(class UItemObject* Item);
	void DropItem(class UItemObject* Item);
	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UPROPERTY()
	class ARelicRunnersCharacter* OwningCharacter;

	UPROPERTY()
	class UInventoryComponent* LocalInventory;

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

	void OnEquippedSlotClick(class UItemObject* EquippedItem);

	UFUNCTION()
	void OnEquippedStatsUpdated(const struct FEquippedStatsSummary& Stats);

	virtual void NativeConstruct() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	UFUNCTION()
	void HandleSortSelected(EInventorySorting Method);

	UFUNCTION()
	void RefreshInventoryUI();

	UFUNCTION()
	void RefreshEquippedUI();

	UFUNCTION()
	void RefreshGoldUI(int32 NewGold);
private:

	
};
