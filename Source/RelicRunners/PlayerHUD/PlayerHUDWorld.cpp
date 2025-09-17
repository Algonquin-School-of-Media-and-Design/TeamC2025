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

#include "PlayerHUDWorld.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "RelicRunners/RelicRunnersCharacter.h"
#include "RelicRunners/PlayerState/RelicRunnersPlayerState.h"

void UPlayerHUDWorld::InitWidgetWithCharacter(ARelicRunnersCharacter* InOwnerCharacter)
{
	if (!InOwnerCharacter) return;

	OwningCharacter = InOwnerCharacter;

}

void UPlayerHUDWorld::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!OwningCharacter) return;

	// Ensure replicated data is current
	const int32 CurrentHealth = OwningCharacter->GetPlayerHealth();
	const int32 MaxHealth = OwningCharacter->GetPlayerMaxHealth();

	if (HealthBar)
	{
		const float Percent = (MaxHealth > 0) ? (float)CurrentHealth / (float)MaxHealth : 0.f;
		HealthBar->SetPercent(Percent);
	}
	if (TB_Health)
	{
		TB_Health->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentHealth, MaxHealth)));
	}

	const int32 CurrentResource = OwningCharacter->GetPlayerResource();
	const int32 MaxResource = OwningCharacter->GetPlayerMaxResource();
	if (ResourceBar)
	{
		const float ResourcePercent = (MaxResource > 0) ? (float)CurrentResource / (float)MaxResource : 0.f;
		ResourceBar->SetPercent(ResourcePercent);
	}

	const int32 CurrentXP = OwningCharacter->GetPlayerXP();
	const int32 RequiredXP = OwningCharacter->GetPlayerXPToLevel();
	if (ExperienceBar)
	{
		const float XPPercent = (RequiredXP > 0) ? (float)CurrentXP / (float)RequiredXP : 0.f;
		ExperienceBar->SetPercent(XPPercent);
	}

	if (TB_Name)
	{
		if (APlayerState* PS = OwningCharacter->GetPlayerState())
		{
			TB_Name->SetText(FText::FromString(FString::Printf(TEXT("%s"), *PS->GetPlayerName())));
		}
	}

	if (TB_Level)
	{
		int32 Level = OwningCharacter->GetPlayerLevel();
		TB_Level->SetText(FText::FromString(FString::Printf(TEXT("%d"), Level)));
	}
	
}