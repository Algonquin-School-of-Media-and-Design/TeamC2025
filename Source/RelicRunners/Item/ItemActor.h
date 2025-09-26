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
#include "GameFramework/Actor.h"
#include "RelicRunners/Interact/InteractInterface.h"
#include "ItemData.h"
#include "ItemActor.generated.h"

class UStaticMeshComponent;
class ULocalFogVolumeComponent;
class UWidgetComponent;
class UUserWidget;
class UInventoryToolTip;

UCLASS()
class RELICRUNNERS_API AItemActor : public AActor, public IInteractInterface
{
    GENERATED_BODY()

public:
    AItemActor();

    void InitializeTooltipWidget();

    // Visual mesh for the dropped item
    UPROPERTY()
    UStaticMeshComponent* StaticMeshComponent;

    // The actual item data struct (replicated)
    UPROPERTY(ReplicatedUsing = OnRep_ItemData)
    struct FItemData ItemData;

    // Fog effect around the item
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual")
    class ULocalFogVolumeComponent* Fog;

    // Tooltip widget shown in world
    UPROPERTY(VisibleAnywhere)
    class UWidgetComponent* TooltipWidgetComponent;

    // Tooltip class to instantiate
    UPROPERTY(EditAnywhere)
    TSubclassOf<UUserWidget> TooltipWidgetClass;

    // Initialize item data on spawn
    UFUNCTION(BlueprintCallable)
    void Initialize(const FItemData& InItemData);

    // Setup mesh, fog, tooltip based on ItemData
    void SetupVisuals();

    // IInteractInterface overrides
    virtual void ShowTooltip_Implementation(bool bShow) override;
    virtual void Interact_Implementation(class ARelicRunnersCharacter* Char) override;
    virtual struct FItemData GetItemData_Implementation() override;

    UFUNCTION(Server, Reliable)
    void Server_HandlePickup(class ARelicRunnersCharacter* Character);

    void HandlePickup(class ARelicRunnersCharacter* Char);
protected:
    virtual void BeginPlay() override;

    // Called when replicated item data arrives on clients
    UFUNCTION()
    void OnRep_ItemData();

    // Replication
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
    virtual void Tick(float DeltaTime) override;
};
