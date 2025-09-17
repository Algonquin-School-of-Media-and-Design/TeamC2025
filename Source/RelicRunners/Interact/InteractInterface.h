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
#include "UObject/Interface.h"
#include "RelicRunners/Item/ItemData.h"
#include "InteractInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UInteractInterface : public UInterface
{
	GENERATED_BODY()
};


class RELICRUNNERS_API IInteractInterface
{
	GENERATED_BODY()
public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    void Interact(class ARelicRunnersCharacter* Char);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    struct FItemData GetItemData();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void ShowTooltip(bool bShow);
};
