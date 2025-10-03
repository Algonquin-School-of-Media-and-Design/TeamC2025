#include "Vendor.h"
#include "Components/CapsuleComponent.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "RelicRunners/RelicRunnersCharacter.h"

AVendor::AVendor()
{
	PrimaryActorTick.bCanEverTick = false;

	// Ensure collide
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

void AVendor::Interact_Implementation(ARelicRunnersCharacter* Char)
{
	if (!Char || !VendorWidgetClass) return;

	APlayerController* PC = Cast<APlayerController>(Char->GetController());
	if (!PC) return;

	// Toggle open/close
	if (ActiveVendorWidget && ActiveVendorWidget->IsInViewport())
	{
		ActiveVendorWidget->RemoveFromParent();
		ActiveVendorWidget = nullptr;

		FInputModeGameOnly GameOnly;
		PC->SetInputMode(GameOnly);
		PC->SetShowMouseCursor(false);
		return;
	}

	ActiveVendorWidget = CreateWidget<UUserWidget>(PC, VendorWidgetClass);
	if (!ActiveVendorWidget) return;

	ActiveVendorWidget->AddToViewport();

	FInputModeGameAndUI GameAndUI;
	GameAndUI.SetHideCursorDuringCapture(false);
	GameAndUI.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PC->SetInputMode(GameAndUI);
	PC->SetShowMouseCursor(true);
}

FItemData AVendor::GetItemData_Implementation()
{
	return FItemData(); 
}

void AVendor::ShowTooltip_Implementation(bool bShow)
{
	// Empty for now
}
