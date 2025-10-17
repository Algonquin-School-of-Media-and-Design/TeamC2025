#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RelicRunners/Item/ItemData.h"
#include "UIVendor.generated.h"

class UTileView;
class UTextBlock;
class UButton;
class UImage;
class UTextureRenderTarget2D;

class AVendor;
class ARelicRunnersCharacter;
class UInventoryComponent;
class UItemObject;

USTRUCT()
struct FVendorCachedEntry
{
	GENERATED_BODY()

	int32     StockIndex = INDEX_NONE;  
	FItemData Data;                     
	int32     Price = 0;                

	FVendorCachedEntry() {}
	FVendorCachedEntry(int32 InIndex, const FItemData& InData, int32 InPrice)
		: StockIndex(InIndex), Data(InData), Price(InPrice) {}
};

UCLASS()
class RELICRUNNERS_API UIVendor : public UUserWidget
{
	GENERATED_BODY()

public:
	// Called by AVendor::Interact(...)
	UFUNCTION(BlueprintCallable, Category = "Vendor")
	void Init(AVendor* InVendor, ARelicRunnersCharacter* InPlayer);

	UFUNCTION(BlueprintCallable, Category = "Vendor")
	void SetPreviewActorImage(UTextureRenderTarget2D* RenderTarget);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget)) UTileView* VendorSlots = nullptr;
	UPROPERTY(meta = (BindWidget)) UTileView* PlayerSlots = nullptr;
	UPROPERTY(meta = (BindWidget)) UTextBlock* TB_Gold = nullptr;
	UPROPERTY(meta = (BindWidget)) UButton* B_Buy = nullptr;
	UPROPERTY(meta = (BindWidget)) UButton* B_Sell = nullptr;
	UPROPERTY(meta = (BindWidget)) UButton* B_Close = nullptr;
	UPROPERTY(meta = (BindWidget)) UImage* I_PlayerPreview = nullptr;

	UPROPERTY(EditAnywhere, Category = "Vendor|UI") TSubclassOf<UUserWidget> PlayerEntryClass;
	UPROPERTY(EditAnywhere, Category = "Vendor|UI") TSubclassOf<UUserWidget> VendorEntryClass;
	UPROPERTY(EditAnywhere, Category = "Vendor|UI") TSubclassOf<UUserWidget> TooltipWidgetClass;

private:
	// References
	UPROPERTY() AVendor* VendorActor = nullptr;
	UPROPERTY() ARelicRunnersCharacter* PlayerChar = nullptr;
	UPROPERTY() UInventoryComponent* PlayerInv = nullptr;

	// Cached vendor stock aligned with VendorSlots entries
	UPROPERTY() TArray<FVendorCachedEntry> CachedStock;

	// Transient UItemObjects for vendor list so tooltips & UniqueID selection work
	UPROPERTY() TArray<UItemObject*> VendorItemObjects;

	// Delegate binding
	void BindInventoryDelegates();
	void UnbindInventoryDelegates();

	UFUNCTION() void OnGoldChanged(int32 NewGold);
	UFUNCTION() void OnInventoryChanged();
	UFUNCTION() void OnEquipmentChanged();

	// ListView click handlers
	UFUNCTION() void HandleVendorItemClicked(UObject* ClickedItem);
	UFUNCTION() void HandlePlayerItemClicked(UObject* ClickedItem);

	// Refreshers
	void RefreshAll();
	void RefreshVendorStock();
	void RefreshPlayerInventory();
	void RefreshGold();

	// Buttons
	UFUNCTION() void OnBuyClicked();
	UFUNCTION() void OnSellClicked();
	UFUNCTION() void OnCloseClicked();

	int32        GetSelectedVendorIndex() const;
	UItemObject* GetSelectedPlayerItem() const;

	bool GetSelectedVendorGuid(FGuid& OutGuid) const;
	bool GetSelectedPlayerGuid(FGuid& OutGuid) const;
	void ReselectVendorByGuid(const FGuid& Guid);
	void ReselectPlayerByGuid(const FGuid& Guid);

	void UpdateButtonStates();

	void TryAnnotateVendorTooltipsWithPrice();
};
