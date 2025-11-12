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
    // use potion if player has one and isn’t at full health
    if (HealthPotionAmount > 0 && PlayerHealth < MaxHealth)
    {
        // Heal player by a percentage of max health
        int HealthToGrant = FMath::RoundToInt(MaxHealth * HealthPercent);
        PlayerHealth += HealthToGrant;
        HealthPotionAmount--;

		// if the player’s health exceeds max health, set it to max health
        if (PlayerHealth > MaxHealth)
        {
            PlayerHealth = MaxHealth;
        }

        // Update potion count UI
        if (TB_HealthPotion)
        {
            TB_HealthPotion->SetText(FText::AsNumber(HealthPotionAmount));
        }
    }
}




