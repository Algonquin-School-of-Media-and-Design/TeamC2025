#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RelicRunners/Interact/InteractInterface.h"
#include "Vendor.generated.h"

class UUserWidget;

UCLASS()
class RELICRUNNERS_API AVendor : public ACharacter, public IInteractInterface
{
	GENERATED_BODY()

public:
	AVendor();

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vendor|UI")
	TSubclassOf<UUserWidget> VendorWidgetClass;

	// Keep a reference so we can toggle/close
	UPROPERTY(Transient)
	UUserWidget* ActiveVendorWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vendor|UI")
	FText InteractPrompt = FText::FromString(TEXT("Press E to Trade"));

public:
	// IInteractInterface
	virtual void Interact_Implementation(class ARelicRunnersCharacter* Char) override;
	virtual FItemData GetItemData_Implementation() override;
	virtual void ShowTooltip_Implementation(bool bShow) override;
	// End IInteractInterface
};
