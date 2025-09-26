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
#include "Blueprint/IUserObjectListEntry.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "InventorySlotsEntry.generated.h"

UCLASS()
class RELICRUNNERS_API UInventorySlotsEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tooltip")
	TSubclassOf<UUserWidget> TooltipWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Popup")
	TSubclassOf<class UInventoryItemOptions> InventoryItemOptionsClass;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_Level;

	UPROPERTY(meta = (BindWidget))
	class UButton* B_ItemFrame;

	UFUNCTION()
	void OnEntryButtonClicked();

	UPROPERTY(meta = (BindWidget))
	class UImage* I_Item;

	UPROPERTY()
	class UItemObject* Item;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

};

