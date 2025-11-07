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
#include "GameFramework/PlayerState.h"
#include "RelicRunnersPlayerState.generated.h"

UCLASS()
class RELICRUNNERS_API ARelicRunnersPlayerState : public APlayerState
{
    GENERATED_BODY()
public:
    UPROPERTY(ReplicatedUsing = OnRep_SelectedClass)
    FName SelectedClass;

    UFUNCTION()
    void OnRep_SelectedClass();

    ARelicRunnersPlayerState();

    ~ARelicRunnersPlayerState();

    void SetSelectedClass(FName NewClass);

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    DECLARE_MULTICAST_DELEGATE_OneParam(FOnSelectedClassChanged, FName);

    FOnSelectedClassChanged OnSelectedClassChanged;

};

