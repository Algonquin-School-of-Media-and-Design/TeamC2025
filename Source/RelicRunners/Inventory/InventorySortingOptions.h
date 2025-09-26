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
	SortByHealth,
	SortByArmor,
	SortByDexterity,
	SortByStrength,
	SortByIntelligence,
	SortByLuck,
	SortBySlots
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

	UPROPERTY(meta = (BindWidget))
	class UButton* B_SortByType;

	UPROPERTY(meta = (BindWidget))
	class UButton* B_SortByRarity;

	UPROPERTY(meta = (BindWidget))
	class UButton* B_SortByLevel;

	UPROPERTY(meta = (BindWidget))
	class UButton* B_SortByHealth;

	UPROPERTY(meta = (BindWidget))
	class UButton* B_SortByArmor;

	UPROPERTY(meta = (BindWidget))
	class UButton* B_SortByDexterity;

	UPROPERTY(meta = (BindWidget))
	class UButton* B_SortByStrength;

	UPROPERTY(meta = (BindWidget))
	class UButton* B_SortByIntelligence;

	UPROPERTY(meta = (BindWidget))
	class UButton* B_SortByLuck;

	UPROPERTY(meta = (BindWidget))
	class UButton* B_SortBySlots;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void SortByItemTypeClicked();

	UFUNCTION()
	void SortByRarityClicked();

	UFUNCTION()
	void SortByLevelClicked();

	UFUNCTION()
	void SortByHealthClicked();

	UFUNCTION()
	void SortByArmorClicked();

	UFUNCTION()
	void SortByDexterityClicked();

	UFUNCTION()
	void SortByStrengthClicked();

	UFUNCTION()
	void SortByIntelligenceClicked();

	UFUNCTION()
	void SortByLuckClicked();

	UFUNCTION()
	void SortBySlotsClicked();
};
