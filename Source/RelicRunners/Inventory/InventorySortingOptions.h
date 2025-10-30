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
#include "InventorySortingOptions.generated.h"

UENUM(BlueprintType)
enum class EInventorySorting : uint8
{
	SortByItemType,
	SortByRarity,
	SortByLevel,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSortingSelected, EInventorySorting, SortingMethod);

UCLASS()
class RELICRUNNERS_API UInventorySortingOptions : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, Category = "Sorting")
	FOnSortingSelected OnSortingSelected;

	static FString GetSortingMethodDisplayName(EInventorySorting Method);
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	UPROPERTY(meta = (BindWidget))
	class UButton* B_SortByType;

	UPROPERTY(meta = (BindWidget))
	class UButton* B_SortByRarity;

	UPROPERTY(meta = (BindWidget))
	class UButton* B_SortByLevel;

	virtual void NativeConstruct() override;

	static class UInventorySortingOptions* CurrentPopup;
	FVector2D InitialScreenPosition;
	void ClosePopup();
	void Setup();

	static void CloseAnyOpenPopup();

	UFUNCTION()
	void SortByItemTypeClicked();

	UFUNCTION()
	void SortByRarityClicked();

	UFUNCTION()
	void SortByLevelClicked();
};
