
#include "AbilityPointCounter.h"
#include "TimerManager.h"
#include "RelicRunners/RelicRunnersCharacter.h"
#include <RelicRunners/Game/RelicRunnersGameInstance.h>
#include <Kismet/GameplayStatics.h>
#include "RelicRunners/Menu/Keybinds.h"

void UAbilityPointCounter::NativeConstruct()
{
	Super::NativeConstruct();
    UpdateBindText();
}

void UAbilityPointCounter::UpdateHUD(int AbilityPoints)
{
	TB_AbilityPoints->SetText(FText::FromString(FString::Printf(TEXT("%d"), AbilityPoints)));
}

void UAbilityPointCounter::UpdateBindText()
{
    UWorld* World = GetWorld();
    if (!World) return;

    URelicRunnersGameInstance* GI = Cast<URelicRunnersGameInstance>(UGameplayStatics::GetGameInstance(World));
    if (!GI || !GI->Keys) return;

    FString DamageKey = GetReadableKeyName(GI->Keys->GetKeyForAction(TEXT("Damage Ability")));
    FString UtilityKey = GetReadableKeyName(GI->Keys->GetKeyForAction(TEXT("Utility Ability")));
    FString DefenceKey = GetReadableKeyName(GI->Keys->GetKeyForAction(TEXT("Defence Ability")));
    FString UltimateKey = GetReadableKeyName(GI->Keys->GetKeyForAction(TEXT("Ultimate Ability")));

    if (TB_DamageAbility)
        TB_DamageAbility->SetText(FText::FromString(DamageKey));

    if (TB_UtilityAbility)
        TB_UtilityAbility->SetText(FText::FromString(UtilityKey));

    if (TB_DefenceAbility)
        TB_DefenceAbility->SetText(FText::FromString(DefenceKey));

    if (TB_UltimateAbility)
        TB_UltimateAbility->SetText(FText::FromString(UltimateKey));
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

