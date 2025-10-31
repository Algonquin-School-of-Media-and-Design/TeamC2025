// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthPotion.h"
#include "TimerManager.h"
#include "Components/TextBlock.h"

void UHealthPotion::NativeConstruct()
{
	Super::NativeConstruct();
}

void UHealthPotion::OnHealthPotionClicked(int& PlayerHealth, int MaxHealth, int& HealthPotionAmount, float HealthPercent)
{
    if (HealthPotionAmount > 0 && PlayerHealth < MaxHealth)
    {
        int HealthToGrant = FMath::RoundToInt(MaxHealth * HealthPercent);
        PlayerHealth += HealthToGrant;
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



