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
#include "EnemyCharacter.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UEnemyHUDWorld::InitWidgetWithCharacter(class ACharacter* InOwnerCharacter)
{
	if (!InOwnerCharacter) return;

	EnemyCharacter = InOwnerCharacter;

}

void UEnemyHUDWorld::UpdateInfo(AEnemyCharacterAI* enemy)
{
	// --- Health ---
	const int32 CurrentHealth = enemy->GetEnemyHealth();
	const int32 MaxHealth = enemy->GetEnemyMaxHealth();

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
	const int32 CurrentResource = enemy->GetEnemyResource();
	const int32 MaxResource = enemy->GetEnemyMaxResource();

	if (ResourceBar)
	{
		const float ResourcePercent = (MaxResource > 0) ? (float)CurrentResource / (float)MaxResource : 0.f;
		ResourceBar->SetPercent(ResourcePercent);
	}

	// --- Name ---
	if (TB_Name)
	{
		FName EnemyName = enemy->GetEnemyName();
		if (!EnemyName.IsNone())
		{
			TB_Name->SetText(FText::FromName(EnemyName));
		}
	}

	// --- Level ---
	if (TB_Level)
	{
		int32 Level = enemy->GetEnemyLevel();
		TB_Level->SetText(FText::AsNumber(Level));
	}
}

void UEnemyHUDWorld::UpdateInfo(AEnemyCharacter* enemy)
{
	// --- Health ---
	const int32 CurrentHealth = enemy->GetCurrentHealth();
	const int32 MaxHealth = enemy->GetMaxHealth();

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
	const int32 CurrentResource = enemy->GetEnemyResource();
	const int32 MaxResource = enemy->GetEnemyMaxResource();

	if (ResourceBar)
	{
		const float ResourcePercent = (MaxResource > 0) ? (float)CurrentResource / (float)MaxResource : 0.f;
		ResourceBar->SetPercent(ResourcePercent);
	}

	// --- Name ---
	if (TB_Name)
	{
		FName EnemyName = enemy->GetEnemyName();
		if (!EnemyName.IsNone())
		{
			TB_Name->SetText(FText::FromName(EnemyName));
		}
	}

	// --- Level ---
	if (TB_Level)
	{
		int32 Level = enemy->GetEnemyLevel();
		TB_Level->SetText(FText::AsNumber(Level));
	}
}

void UEnemyHUDWorld::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!EnemyCharacter) return;

	if (AEnemyCharacterAI* tEnemy = Cast<AEnemyCharacterAI>(EnemyCharacter))
	{
		UpdateInfo(tEnemy);
	}

	if (AEnemyCharacter* enemy = Cast<AEnemyCharacter>(EnemyCharacter))
	{
		UpdateInfo(enemy);
	}
}