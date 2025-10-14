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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Price = 50; // tweak as you like
};

UCLASS()
class RELICRUNNERS_API AVendor : public ACharacter, public IInteractInterface
{
	GENERATED_BODY()

public:
	AVendor();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vendor|UI")
	TSubclassOf<UUserWidget> VendorWidgetClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vendor|UI")
	FText InteractPrompt = FText::FromString(TEXT("Press E to Trade"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vendor|Stock")
	int32 ItemsForSaleCount = 8;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vendor|Stock")
	TSubclassOf<UItemMeshData> ItemMeshDataClass;

	UFUNCTION(BlueprintCallable, Category = "Vendor")
	void GetCurrentStock(TArray<FVendorEntry>& OutStock) const { OutStock = Stock; }

	// Core server-side logic
	UFUNCTION(BlueprintCallable, Category = "Vendor")
	bool BuyItemByIndex(int32 Index, ARelicRunnersCharacter* Buyer);

	UFUNCTION(BlueprintCallable, Category = "Vendor")
	bool SellItemByGuid(FGuid ItemGuid, ARelicRunnersCharacter* Seller);


	UFUNCTION(BlueprintCallable, Category = "Vendor")
	void RerollStock();

	UFUNCTION(Server, Reliable)
	void Server_BuyItemByIndex(int32 Index, ARelicRunnersCharacter* Buyer);

	UFUNCTION(Server, Reliable)
	void Server_SellItemByGuid(FGuid ItemGuid, ARelicRunnersCharacter* Seller);

	// IInteractInterface
	virtual void Interact_Implementation(ARelicRunnersCharacter* Char) override;
	virtual FItemData GetItemData_Implementation() override;
	virtual void ShowTooltip_Implementation(bool bShow) override; // Empty

protected:
	virtual void BeginPlay() override;

	void GenerateStock();

	int32 ComputePrice(const FItemData& Data) const;

	// Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Generated stock
	UPROPERTY(ReplicatedUsing = OnRep_Stock, VisibleAnywhere, BlueprintReadOnly, Category = "Vendor|Stock")
	TArray<FVendorEntry> Stock;

	UFUNCTION()
	void OnRep_Stock() { /* hook if you want to notify UI later */ }

	// Cached (non-replicated)
	UPROPERTY(Transient)
	UItemMeshData* ItemMeshData = nullptr;
};
