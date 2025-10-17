
#include "AbilityPointCounter.h"
#include "TimerManager.h"
#include "RelicRunners/RelicRunnersCharacter.h"

void UAbilityPointCounter::NativeConstruct()
{
	Super::NativeConstruct();

}

void UAbilityPointCounter::UpdateHUD(int AbilityPoints)
{
	TB_AbilityPoints->SetText(FText::FromString(FString::Printf(TEXT("%d"), AbilityPoints)));
}

void UAbilityPointCounter::RunCooldown(UTextBlock* CooldownText, float& RemainingTime, FTimerHandle& TimerHandle, float CooldownTime)
{
    if (!CooldownText || CooldownTime <= 0.f) return;

    if (RemainingTime > 0.f)
        return;

    RemainingTime = CooldownTime;
    CooldownText->SetText(FText::AsNumber(FMath::CeilToInt(RemainingTime)));

    GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
    GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this, CooldownText, &RemainingTime, &TimerHandle]()
        {
            RemainingTime -= 1.0f;

            if (RemainingTime <= 0.f)
            {
                CooldownText->SetText(FText::FromString(TEXT("")));
                GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
            }
            else
            {
                CooldownText->SetText(FText::AsNumber(FMath::CeilToInt(RemainingTime)));
            }
        },1.0f,true
    );
}
void UAbilityPointCounter::StartDamageCooldown(float CooldownTime)
{
    RunCooldown(TB_DamageCooldown, DamageRemainingTime, DamageCooldownHandle, CooldownTime);
}

void UAbilityPointCounter::StartDefenceCooldown(float CooldownTime)
{
    RunCooldown(TB_DefenceCooldown, DefenceRemainingTime, DefenceCooldownHandle, CooldownTime);
}

void UAbilityPointCounter::StartUtilityCooldown(float CooldownTime)
{
    RunCooldown(TB_UtilityCooldown, UtilityRemainingTime, UtilityCooldownHandle, CooldownTime);
}

void UAbilityPointCounter::StartUltimateCooldown(float CooldownTime)
{
    RunCooldown(TB_UltimateCooldown, UltimateRemainingTime, UltimateCooldownHandle, CooldownTime);
}

