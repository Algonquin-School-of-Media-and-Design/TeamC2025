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

	VendorWidget = CreateWidget<UIVendor>(GetWorld(), VendorWidgetClass);
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
	
	return false;
}

bool AVendor::SellItemByGuid(FGuid ItemGuid, ARelicRunnersCharacter* Seller)
{
	return false;
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
			if (VendorWidget->IsVisible())
			{
				VendorWidget->SetVisibility(ESlateVisibility::Hidden);
				VendorWidget->SetIsEnabled(false);
				PC->SetInputMode(FInputModeGameOnly());
				PC->SetShowMouseCursor(false);
			}
			else
			{
				// Initialize the vendor UI with this vendor and the interacting player
				VendorWidget->Init(this, Char);

				// Add UI to viewport
				VendorWidget->AddToViewport();

				VendorWidget->SetVisibility(ESlateVisibility::Visible);
				VendorWidget->SetIsEnabled(true);
				PC->SetInputMode(FInputModeGameAndUI());
				PC->SetShowMouseCursor(true);
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
