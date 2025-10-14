#include "Vendor.h"

#include "Components/CapsuleComponent.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"

#include "RelicRunners/RelicRunnersCharacter.h"
#include "RelicRunners/Inventory/InventoryComponent.h"
#include "RelicRunners/Item/ItemStats.h"
#include "RelicRunners/Item/ItemData.h"
#include "RelicRunners/Inventory/Inventory.h"

#include "RelicRunners/Vendor/UIVendor.h"

AVendor::AVendor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Capsule->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	}

	if (GetMesh())
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	}
}

void AVendor::BeginPlay()
{
	Super::BeginPlay();

	if (ItemMeshDataClass)
	{
		ItemMeshData = ItemMeshDataClass->GetDefaultObject<UItemMeshData>();
	}

	if (HasAuthority())
	{
		GenerateStock();
	}
}

void AVendor::GenerateStock()
{
	Stock.Empty();

	if (!ItemMeshData)
	{
		// Still allow stock using default values if you want; otherwise just return
		return;
	}

	for (int32 i = 0; i < ItemsForSaleCount; ++i)
	{
		FVendorEntry Entry;
		Entry.Item = ItemStats::CreateRandomItemData(ItemMeshData);
		Entry.Price = ComputePrice(Entry.Item);
		Stock.Add(Entry);
	}
}

int32 AVendor::ComputePrice(const FItemData& Data) const
{
	int32 Base = 50;
	int32 RarityBump = 0;


	return Base + RarityBump;
}

void AVendor::RerollStock()
{
	if (HasAuthority())
	{
		GenerateStock();
	}
}

bool AVendor::BuyItemByIndex(int32 Index, ARelicRunnersCharacter* Buyer)
{
	// Short if's
	if (!Buyer || !Buyer->GetInventoryComponent()) return false;
	if (!HasAuthority()) return false;                // Server only
	if (!Stock.IsValidIndex(Index)) return false;

	UInventoryComponent* Inv = Buyer->GetInventoryComponent();
	FVendorEntry& Entry = Stock[Index];

	// Spend gold first
	if (!Inv->TryChangeGold(-Entry.Price))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Vendor] Not enough gold to buy."));
		return false;
	}

	if (UItemObject* NewItem = ItemStats::CreateItemFromData(Entry.Item, Inv))
	{
		Inv->AddItem(NewItem);
		// Remove from vendor stock
		Stock.RemoveAt(Index);
		return true;
	}

	// Refund if creation failed for any reason
	Inv->TryChangeGold(+Entry.Price);
	return false;
}

bool AVendor::SellItemByGuid(FGuid ItemGuid, ARelicRunnersCharacter* Seller)
{
	if (!Seller || !Seller->GetInventoryComponent()) return false;
	if (!HasAuthority()) return false;

	UInventoryComponent* Inv = Seller->GetInventoryComponent();

	// Find the item in inventory
	int32 FoundIndex = INDEX_NONE;
	UItemObject* FoundItem = nullptr;

	for (int32 i = 0; i < Inv->InventoryItems.Num(); ++i)
	{
		if (Inv->InventoryItems[i] && Inv->InventoryItems[i]->ItemData.UniqueID == ItemGuid)
		{
			FoundIndex = i;
			FoundItem = Inv->InventoryItems[i];
			break;
		}
	}

	if (FoundIndex == INDEX_NONE || !FoundItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Vendor] Sell failed: item not found in inventory."));
		return false;
	}

	// Determine a sell price. Simple: half of what we would list it for.
	const int32 SellPrice = FMath::Max(1, ComputePrice(FoundItem->ItemData) / 2);

	// Pay the player and remove the item
	Inv->TryChangeGold(+SellPrice);
	Inv->InventoryItems.RemoveAt(FoundIndex);

	// (Optional) add the sold item to vendor stock for fun:
	FVendorEntry NewEntry;
	NewEntry.Item = FoundItem->ItemData;
	NewEntry.Price = ComputePrice(NewEntry.Item);
	Stock.Add(NewEntry);

	return true;
}

void AVendor::Server_BuyItemByIndex_Implementation(int32 Index, ARelicRunnersCharacter* Buyer)
{
	BuyItemByIndex(Index, Buyer);
}

void AVendor::Server_SellItemByGuid_Implementation(FGuid ItemGuid, ARelicRunnersCharacter* Seller)
{
	SellItemByGuid(ItemGuid, Seller);
}

void AVendor::Interact_Implementation(ARelicRunnersCharacter* Char)
{
	if (!Char) return;

	if (APlayerController* PC = Cast<APlayerController>(Char->GetController()))
	{
		if (VendorWidgetClass)
		{
			if (UUserWidget* CreatedWidget = CreateWidget<UUserWidget>(PC, VendorWidgetClass))
			{
				if (UIVendor* VendorUI = Cast<UIVendor>(CreatedWidget))
				{
					// Initialize the vendor UI with this vendor and the interacting player
					VendorUI->Init(this, Char);

					// Add UI to viewport
					VendorUI->AddToViewport();

					// Switch input to game+UI (cursor visible)
					FInputModeGameAndUI InputMode;
					InputMode.SetHideCursorDuringCapture(false);
					InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
					PC->SetInputMode(InputMode);
					PC->SetShowMouseCursor(true);
				}
			}
		}
	}
}

FItemData AVendor::GetItemData_Implementation()
{
	return FItemData(); // vendor doesn’t need this
}

void AVendor::ShowTooltip_Implementation(bool bShow)
{
	// Left empty for now (later: “Press E to Trade”)
}

// Replication
void AVendor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AVendor, Stock);
}
