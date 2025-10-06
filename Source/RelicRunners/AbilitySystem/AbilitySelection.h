// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySelection.generated.h"

/**
 * 
 */
UCLASS()
class RELICRUNNERS_API UAbilitySelection : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UButton* B_Upgrade1;

	UPROPERTY(meta = (BindWidget))
	class UButton* B_Upgrade2;

	UPROPERTY(meta = (BindWidget))
	class UButton* B_Upgrade3;

	UPROPERTY(meta = (BindWidget))
	class UButton* B_Upgrade4;



	UPROPERTY()
	class ARelicRunnersCharacter* OwningCharacter;


	void SpendAbilityPoints();

	UFUNCTION()
	void Upgrade1Clicked();

	UFUNCTION()
	void Upgrade2Clicked();

	UFUNCTION()
	void Upgrade3Clicked();

	UFUNCTION()
	void Upgrade4Clicked();

};
