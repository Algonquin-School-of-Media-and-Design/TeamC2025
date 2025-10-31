// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthPotion.generated.h"

/**
 * 
 */
UCLASS()
class RELICRUNNERS_API UHealthPotion : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	void OnHealthPotionClicked(int& PlayerHealth, int MaxHealth, int& HealthPotionAmount, float HealthPercent);

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_HealthPotion;


};
