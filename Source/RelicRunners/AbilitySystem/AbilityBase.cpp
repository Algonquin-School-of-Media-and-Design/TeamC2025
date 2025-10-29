#include "AbilityBase.h"

AAbilityBase::AAbilityBase()
{
    bIsOnCooldown = false;
    bCanBeInterrupted = true;
    Cooldown = 0.f;
    Duration = 0.f;
    DamageAmount = 0.f;
    AreaRadius = 0.f;
    ConeAngle = 0.f;
    AbilityName = NAME_None;
    AbilityCategory = EAbilityCategory::None;
    TargetType = EAbilityTargetType::None;
    OwnerActor = nullptr;
}

void AAbilityBase::ActivateAbility() {}
void AAbilityBase::EndAbility() {}
bool AAbilityBase::CanActivate() const { return true; }
FName AAbilityBase::GetAbilityName() const { return AbilityName; }
