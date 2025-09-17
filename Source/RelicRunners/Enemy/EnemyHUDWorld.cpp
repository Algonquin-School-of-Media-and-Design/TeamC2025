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

#include "EnemyHUDWorld.h"
#include "EnemyCharacterAI.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UEnemyHUDWorld::InitWidgetWithCharacter(AEnemyCharacterAI* InOwnerCharacter)
{
	if (!InOwnerCharacter) return;

	EnemyCharacter = InOwnerCharacter;

}

void UEnemyHUDWorld::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!EnemyCharacter) return;

	// --- Health ---
	const int32 CurrentHealth = EnemyCharacter->GetEnemyHealth();
	const int32 MaxHealth = EnemyCharacter->GetEnemyMaxHealth();

	if (HealthBar)
	{
		const float Percent = (MaxHealth > 0) ? (float)CurrentHealth / (float)MaxHealth : 0.f;
		HealthBar->SetPercent(Percent);
	}

	if (TB_Health)
	{
		TB_Health->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentHealth, MaxHealth)));
	}

	// --- Resource ---
	const int32 CurrentResource = EnemyCharacter->GetEnemyResource();
	const int32 MaxResource = EnemyCharacter->GetEnemyMaxResource();

	if (ResourceBar)
	{
		const float ResourcePercent = (MaxResource > 0) ? (float)CurrentResource / (float)MaxResource : 0.f;
		ResourceBar->SetPercent(ResourcePercent);
	}

	// --- Name ---
	if (TB_Name)
	{
		FName EnemyName = EnemyCharacter->GetEnemyName();
		if (!EnemyName.IsNone())
		{
			TB_Name->SetText(FText::FromName(EnemyName));
		}
	}

	// --- Level ---
	if (TB_Level)
	{
		int32 Level = EnemyCharacter->GetEnemyLevel();
		TB_Level->SetText(FText::AsNumber(Level));
	}
}