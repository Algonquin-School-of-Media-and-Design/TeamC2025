// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthPotion.h"
#include "TimerManager.h"
#include "Components/TextBlock.h"

void UHealthPotion::NativeConstruct()
{
	Super::NativeConstruct();
}

void UHealthPotion::OnHealthPotionClicked(int& PlayerHealth, int MaxHealth, int& HealthPotionAmount, int HealthGranted)
{
    if (HealthPotionAmount > 0 && PlayerHealth < MaxHealth)
    {
        PlayerHealth += HealthGranted;
        HealthPotionAmount--;


        if (PlayerHealth > MaxHealth)
        {
            PlayerHealth = MaxHealth;
        }

        if (TB_HealthPotion)
        {
            TB_HealthPotion->SetText(FText::AsNumber(HealthPotionAmount));
        }
    }
}



