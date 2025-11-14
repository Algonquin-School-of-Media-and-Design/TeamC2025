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
		return;
	}

	for (int32 i = 0; i < 10; ++i)
	{
		FVendorEntry Entry;
		Entry.Item = ItemStats::CreateRandomItemData(ItemMeshData);
		Stock.Add(Entry);
	}
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

void AVendor::Interact_Implementation(ARelicRunnersCharacter* Char)
{
	if (!Char) return;

	if (APlayerController* PC = Cast<APlayerController>(Char->GetController()))
	{
		
	}
}

FItemData AVendor::GetItemData_Implementation()
{
	return FItemData();
}

void AVendor::ShowTooltip_Implementation(bool bShow)
{
	
}

void AVendor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AVendor, Stock);
}
