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

#include "PlayerHUD.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void UPlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPlayerHUD::UpdateHUD(float CurrentHealth, float MaxHealth, float CurrentResource, float MaxResource, int CurrentLevel, float CurrentXP, float XPToLevel)
{
	if (!HealthBar || !ResourceBar || !ExperienceBar || !TB_Health || !TB_Resource || !TB_Level || !TB_Experience)
	{
		UE_LOG(LogTemp, Warning, TEXT("HUD component not initialized properly"));
		return;
	}

	float HealthPercent = MaxHealth > 0 ? CurrentHealth / MaxHealth : 0.f;
	float ResourcePercent = MaxResource > 0 ? CurrentResource / MaxResource : 0.f;
	float ExperiencePercent = XPToLevel > 0 ? CurrentXP / XPToLevel : 0.f;

	HealthBar->SetPercent(HealthPercent);
	ResourceBar->SetPercent(ResourcePercent);
	ExperienceBar->SetPercent(ExperiencePercent);

	TB_Health->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), FMath::RoundToInt(CurrentHealth), FMath::RoundToInt(MaxHealth))));
	TB_Resource->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), FMath::RoundToInt(CurrentResource), FMath::RoundToInt(MaxResource))));
	TB_Level->SetText(FText::FromString(FString::Printf(TEXT("Level %d"), CurrentLevel)));
	TB_Experience->SetText(FText::FromString(FString::Printf(TEXT("%d / %d XP"), FMath::RoundToInt(CurrentXP), FMath::RoundToInt(XPToLevel))));
	
}

void UPlayerHUD::UpdateHUDHealth(float CurrentHealth, float MaxHealth)
{
	if (!HealthBar || !TB_Health)
	{
		UE_LOG(LogTemp, Warning, TEXT("HUD component not initialized properly"));
		return;
	}

	float HealthPercent = MaxHealth > 0 ? CurrentHealth / MaxHealth : 0.f;

	HealthBar->SetPercent(HealthPercent);
	TB_Health->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), FMath::RoundToInt(CurrentHealth), FMath::RoundToInt(MaxHealth))));
}

void UPlayerHUD::UpdateHUDResource(float CurrentResource, float MaxResource)
{
	if (!ResourceBar || !TB_Resource)
	{
		UE_LOG(LogTemp, Warning, TEXT("HUD component not initialized properly"));
		return;
	}

	float ResourcePercent = MaxResource > 0 ? CurrentResource / MaxResource : 0.f;

	ResourceBar->SetPercent(ResourcePercent);
	TB_Resource->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), FMath::RoundToInt(CurrentResource), FMath::RoundToInt(MaxResource))));
}

void UPlayerHUD::UpdateHUDXP(int CurrentLevel, float CurrentXP, float XPToLevel)
{
	if (!ExperienceBar || !TB_Level || !TB_Experience)
	{
		UE_LOG(LogTemp, Warning, TEXT("HUD component not initialized properly"));
		return;
	}

	float ExperiencePercent = XPToLevel > 0 ? CurrentXP / XPToLevel : 0.f;
	ExperienceBar->SetPercent(ExperiencePercent);
	TB_Level->SetText(FText::FromString(FString::Printf(TEXT("Level %d"), CurrentLevel)));
	TB_Experience->SetText(FText::FromString(FString::Printf(TEXT("%d / %d XP"), FMath::RoundToInt(CurrentXP), FMath::RoundToInt(XPToLevel))));

}