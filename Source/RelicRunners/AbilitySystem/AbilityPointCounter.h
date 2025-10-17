#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "AbilityPointCounter.generated.h"

UCLASS()
class RELICRUNNERS_API UAbilityPointCounter : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;


    FTimerHandle DamageCooldownHandle;
    FTimerHandle DefenceCooldownHandle;
    FTimerHandle UtilityCooldownHandle;
    FTimerHandle UltimateCooldownHandle;


    float DamageRemainingTime = 0.f;
    float DefenceRemainingTime = 0.f;
    float UtilityRemainingTime = 0.f;
    float UltimateRemainingTime = 0.f;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TB_DamageCooldown;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TB_DefenceCooldown;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TB_UtilityCooldown;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TB_UltimateCooldown;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TB_AbilityPoints;

public:


    UFUNCTION(BlueprintCallable)
    void UpdateHUD(int AbilityPoints);

    void RunCooldown(UTextBlock* CooldownText, float& RemainingTime, FTimerHandle& TimerHandle, float CooldownTime);


    UFUNCTION(BlueprintCallable)
    void StartDamageCooldown(float CooldownTime);

    UFUNCTION(BlueprintCallable)
    void StartDefenceCooldown(float CooldownTime);

    UFUNCTION(BlueprintCallable)
    void StartUtilityCooldown(float CooldownTime);

    UFUNCTION(BlueprintCallable)
    void StartUltimateCooldown(float CooldownTime);

};