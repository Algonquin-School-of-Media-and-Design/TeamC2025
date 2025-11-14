#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RelicRunners/Interact/InteractInterface.h"
#include "Vendor.generated.h"

class UUserWidget;
class UItemMeshData;
struct FItemData;
class UInventoryComponent;
class ARelicRunnersCharacter;


USTRUCT(BlueprintType)
struct FVendorEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemData Item;
};

UCLASS()
class RELICRUNNERS_API AVendor : public ACharacter, public IInteractInterface
{
	GENERATED_BODY()

public:
	AVendor();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vendor|Stock")
	TSubclassOf<UItemMeshData> ItemMeshDataClass;

	UFUNCTION(BlueprintCallable, Category = "Vendor")
	void RerollStock();

	bool BuyItemByIndex(int32 Index, ARelicRunnersCharacter* Buyer);

	bool SellItemByGuid(FGuid ItemGuid, ARelicRunnersCharacter* Seller);

	// IInteractInterface
	virtual void Interact_Implementation(ARelicRunnersCharacter* Char) override;
	virtual FItemData GetItemData_Implementation() override;
	virtual void ShowTooltip_Implementation(bool bShow) override; // Empty

protected:
	virtual void BeginPlay() override;

	void GenerateStock();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Generated stock
	UPROPERTY(ReplicatedUsing = OnRep_Stock, VisibleAnywhere, BlueprintReadOnly, Category = "Vendor|Stock")
	TArray<FVendorEntry> Stock;

	UFUNCTION()
	void OnRep_Stock() { }

	// Cached (non-replicated)
	UPROPERTY(Transient)
	UItemMeshData* ItemMeshData = nullptr;
};
