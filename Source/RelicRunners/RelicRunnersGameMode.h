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
#include "GameFramework/GameModeBase.h"
#include "RelicRunnersGameMode.generated.h"

UCLASS(minimalapi)
class ARelicRunnersGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARelicRunnersGameMode();
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void PostSeamlessTravel() override;
	ACharacter* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot);
	UPROPERTY(EditDefaultsOnly, Category = "Preview")
	TSubclassOf<class APlayerPreview> PlayerPreviewClass;

};



