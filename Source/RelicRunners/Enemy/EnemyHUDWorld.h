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
#include "EnemyHUDWorld.generated.h"

/**
 * 
 */
UCLASS()
class RELICRUNNERS_API UEnemyHUDWorld : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void InitWidgetWithCharacter(class ACharacter* InOwnerCharacter);

	void UpdateInfo(class AEnemyCharacterAI* enemy);
	void UpdateInfo(class AEnemyCharacter* enemy);

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_Health;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_Level;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TB_Name;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ResourceBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

protected:

	UPROPERTY()
	class ACharacter* EnemyCharacter;
};
