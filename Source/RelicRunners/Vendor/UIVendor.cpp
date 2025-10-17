#include "UIVendor.h"

#include "Components/TileView.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "RelicRunners/RelicRunnersCharacter.h"
#include "RelicRunners/PlayerController/RelicRunnersPlayerController.h"
#include "RelicRunners/Inventory/InventoryComponent.h"
#include "RelicRunners/Inventory/InventorySlotsEntry.h"
#include "RelicRunners/Inventory/InventoryToolTip.h"
#include "RelicRunners/Item/ItemData.h"
#include "RelicRunners/Item/ItemStats.h"
#include "RelicRunners/Item/ItemMeshData.h"
#include "RelicRunners/Vendor/Vendor.h"

#include "Engine/TextureRenderTarget2D.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Widgets/Views/STableViewBase.h" 

void UIVendor::Init(AVendor* InVendor, ARelicRunnersCharacter* InPlayer)
{
	VendorActor = InVendor;
	PlayerChar = InPlayer;
	PlayerInv = (PlayerChar ? PlayerChar->GetInventoryComponent() : nullptr);

	BindInventoryDelegates();
	RefreshAll();
}

void UIVendor::NativeConstruct()
{
	Super::NativeConstruct();

	if (VendorSlots) VendorSlots->SetSelectionMode(ESelectionMode::Single);
	if (PlayerSlots) PlayerSlots->SetSelectionMode(ESelectionMode::Single);

	// Click handlers
	if (VendorSlots) VendorSlots->OnItemClicked().AddUObject(this, &UIVendor::HandleVendorItemClicked);
	if (PlayerSlots) PlayerSlots->OnItemClicked().AddUObject(this, &UIVendor::HandlePlayerItemClicked);

	// Buttons
	if (B_Buy)   B_Buy->OnClicked.AddDynamic(this, &UIVendor::OnBuyClicked);
	if (B_Sell)  B_Sell->OnClicked.AddDynamic(this, &UIVendor::OnSellClicked);
	if (B_Close) B_Close->OnClicked.AddDynamic(this, &UIVendor::OnCloseClicked);

	if (!PlayerChar)
	{
		if (APlayerController* PC = GetOwningPlayer())
		{
			PlayerChar = Cast<ARelicRunnersCharacter>(PC->GetPawn());
			PlayerInv = PlayerChar ? PlayerChar->GetInventoryComponent() : nullptr;
		}
	}

	BindInventoryDelegates();
	RefreshAll();
	UpdateButtonStates();
}

void UIVendor::NativeDestruct()
{
	if (VendorSlots) VendorSlots->OnItemClicked().RemoveAll(this);
	if (PlayerSlots) PlayerSlots->OnItemClicked().RemoveAll(this);

	UnbindInventoryDelegates();
	Super::NativeDestruct();
}

void UIVendor::BindInventoryDelegates()
{
	if (!PlayerInv) return;

	PlayerInv->OnGoldChanged.RemoveAll(this);
	PlayerInv->OnInventoryChanged.RemoveAll(this);
	PlayerInv->OnEquipmentChanged.RemoveAll(this);

	PlayerInv->OnGoldChanged.AddDynamic(this, &UIVendor::OnGoldChanged);
	PlayerInv->OnInventoryChanged.AddDynamic(this, &UIVendor::OnInventoryChanged);
	PlayerInv->OnEquipmentChanged.AddDynamic(this, &UIVendor::OnEquipmentChanged);
}

void UIVendor::UnbindInventoryDelegates()
{
	if (!PlayerInv) return;

	PlayerInv->OnGoldChanged.RemoveAll(this);
	PlayerInv->OnInventoryChanged.RemoveAll(this);
	PlayerInv->OnEquipmentChanged.RemoveAll(this);
}

void UIVendor::OnGoldChanged(int32 /*NewGold*/)
{
	RefreshGold();
	UpdateButtonStates();
}

void UIVendor::OnInventoryChanged()
{
	RefreshPlayerInventory();
	UpdateButtonStates();
}

void UIVendor::OnEquipmentChanged()
{
	// If you want preview updates, call SetPreviewActorImage() from your PC as in inventory UI.
}


void UIVendor::HandleVendorItemClicked(UObject* ClickedItem)
{
	if (!VendorSlots || !ClickedItem) return;
	VendorSlots->SetSelectedItem(ClickedItem);
	UpdateButtonStates();
}

void UIVendor::HandlePlayerItemClicked(UObject* ClickedItem)
{
	if (!PlayerSlots || !ClickedItem) return;
	PlayerSlots->SetSelectedItem(ClickedItem);
	UpdateButtonStates();
}

// Refresh
void UIVendor::RefreshAll()
{
	RefreshGold();
	RefreshVendorStock();
	RefreshPlayerInventory();
}

void UIVendor::RefreshGold()
{
	if (TB_Gold && PlayerInv)
	{
		TB_Gold->SetText(FText::AsNumber(PlayerInv->GetGold()));
	}
}

void UIVendor::RefreshVendorStock()
{
	if (!VendorActor || !VendorSlots) return;

	FGuid PrevGuid;
	const bool bHadSelection = GetSelectedVendorGuid(PrevGuid);

	TArray<FVendorEntry> ServerStock;
	VendorActor->GetCurrentStock(ServerStock);

	CachedStock.Reset();
	VendorItemObjects.Reset();

	for (int32 i = 0; i < ServerStock.Num(); ++i)
	{
		const FVendorEntry& E = ServerStock[i];
		CachedStock.Emplace(i, E.Item, E.Price);

		UItemObject* TempItem = ItemStats::CreateItemFromData(E.Item, this);
		VendorItemObjects.Add(TempItem);
	}

	VendorSlots->ClearListItems();
	for (UItemObject* Obj : VendorItemObjects)
	{
		VendorSlots->AddItem(Obj);
	}

	if (bHadSelection)
	{
		ReselectVendorByGuid(PrevGuid);
	}
	else
	{
		VendorSlots->ClearSelection();
	}

	UpdateButtonStates();
	TryAnnotateVendorTooltipsWithPrice();
}

void UIVendor::RefreshPlayerInventory()
{
	if (!PlayerInv || !PlayerSlots) return;

	FGuid PrevGuid;
	const bool bHadSelection = GetSelectedPlayerGuid(PrevGuid);

	PlayerSlots->ClearListItems();
	for (UItemObject* Item : PlayerInv->InventoryItems)
	{
		PlayerSlots->AddItem(Item);
	}

	if (bHadSelection)
	{
		ReselectPlayerByGuid(PrevGuid);
	}
	else
	{
		PlayerSlots->ClearSelection();
	}

	UpdateButtonStates();
}

// Buttons
void UIVendor::OnBuyClicked()
{
	if (!VendorActor || !PlayerChar) return;

	const int32 SelIndex = GetSelectedVendorIndex();
	if (SelIndex == INDEX_NONE)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[VendorUI] No vendor item selected."));
		UpdateButtonStates();
		return;
	}

	VendorActor->Server_BuyItemByIndex(SelIndex, PlayerChar);

	// Local polish
	RefreshVendorStock();
	RefreshPlayerInventory();
	RefreshGold();
	UpdateButtonStates();
}

void UIVendor::OnSellClicked()
{
	if (!VendorActor || !PlayerChar) return;

	UItemObject* Selected = GetSelectedPlayerItem();
	if (!Selected)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[VendorUI] No player item selected to sell."));
		UpdateButtonStates();
		return;
	}

	VendorActor->Server_SellItemByGuid(Selected->GetUniqueID(), PlayerChar);

	// Local polish
	RefreshVendorStock();
	RefreshPlayerInventory();
	RefreshGold();
	UpdateButtonStates();
}

void UIVendor::OnCloseClicked()
{
	RemoveFromParent();

	if (APlayerController* PC = GetOwningPlayer())
	{
		FInputModeGameOnly GameOnly;
		PC->SetInputMode(GameOnly);
		PC->SetShowMouseCursor(false);
	}
}

int32 UIVendor::GetSelectedVendorIndex() const
{
	if (!VendorSlots) return INDEX_NONE;

	UObject* SelectedObj = VendorSlots->GetSelectedItem();
	if (!SelectedObj) return INDEX_NONE;

	const UItemObject* SelItem = Cast<UItemObject>(SelectedObj);
	if (!SelItem) return INDEX_NONE;

	const FGuid Guid = SelItem->GetUniqueID();
	for (const FVendorCachedEntry& CE : CachedStock)
	{
		if (CE.Data.UniqueID == Guid)
		{
			return CE.StockIndex;
		}
	}
	return INDEX_NONE;
}

UItemObject* UIVendor::GetSelectedPlayerItem() const
{
	if (!PlayerSlots) return nullptr;
	return Cast<UItemObject>(PlayerSlots->GetSelectedItem());
}

bool UIVendor::GetSelectedVendorGuid(FGuid& OutGuid) const
{
	if (!VendorSlots) return false;
	if (UObject* Sel = VendorSlots->GetSelectedItem())
	{
		if (const UItemObject* Item = Cast<UItemObject>(Sel))
		{
			OutGuid = Item->GetUniqueID();
			return true;
		}
	}
	return false;
}

bool UIVendor::GetSelectedPlayerGuid(FGuid& OutGuid) const
{
	if (!PlayerSlots) return false;
	if (UObject* Sel = PlayerSlots->GetSelectedItem())
	{
		if (const UItemObject* Item = Cast<UItemObject>(Sel))
		{
			OutGuid = Item->GetUniqueID();
			return true;
		}
	}
	return false;
}

void UIVendor::ReselectVendorByGuid(const FGuid& Guid)
{
	if (!VendorSlots) return;
	for (UItemObject* Obj : VendorItemObjects)
	{
		if (Obj && Obj->GetUniqueID() == Guid)
		{
			VendorSlots->SetSelectedItem(Obj);
			return;
		}
	}
}

void UIVendor::ReselectPlayerByGuid(const FGuid& Guid)
{
	if (!PlayerSlots || !PlayerInv) return;
	for (UItemObject* Obj : PlayerInv->InventoryItems)
	{
		if (Obj && Obj->GetUniqueID() == Guid)
		{
			PlayerSlots->SetSelectedItem(Obj);
			return;
		}
	}
}

void UIVendor::UpdateButtonStates()
{
	const bool bHasVendorSelection = (VendorSlots && VendorSlots->GetSelectedItem() != nullptr);
	const bool bHasPlayerSelection = (PlayerSlots && PlayerSlots->GetSelectedItem() != nullptr);

	if (B_Buy)  B_Buy->SetIsEnabled(bHasVendorSelection);
	if (B_Sell) B_Sell->SetIsEnabled(bHasPlayerSelection);
}

void UIVendor::SetPreviewActorImage(UTextureRenderTarget2D* RenderTarget)
{
	if (!RenderTarget || !I_PlayerPreview) return;

	UMaterialInterface* BaseMaterial = LoadObject<UMaterialInterface>(
		nullptr,
		TEXT("/Game/ThirdPerson/Blueprints/PlayerPreview/M_PlayerPreview.M_PlayerPreview")
	);
	if (!BaseMaterial) return;

	UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(BaseMaterial, this);
	if (!DynMat) return;

	DynMat->SetTextureParameterValue(FName("PreviewTexture"), RenderTarget);
	I_PlayerPreview->SetBrushFromMaterial(DynMat);
}

void UIVendor::TryAnnotateVendorTooltipsWithPrice()
{
	
}
