#include "ImpunityAbility.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"

UImpunityAbility::UImpunityAbility()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    Duration = 5.0f;
    CooldownDuration = 8.0f;
}

void UImpunityAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    ACharacter* AvatarChar = Cast<ACharacter>(GetAvatarActorFromActorInfo());
    if (!AvatarChar)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    bIsActive = true;
    OriginalSpeed = AvatarChar->GetCharacterMovement()->MaxWalkSpeed;
    AvatarChar->GetCharacterMovement()->MaxWalkSpeed *= 1.75f;
    AvatarChar->GetWorldTimerManager().SetTimer(SpeedResetTimer, FTimerDelegate::CreateUObject(this, &UImpunityAbility::EndAbility, Handle, ActorInfo, ActivationInfo, true, false), Duration, false);
}

void UImpunityAbility::ResetSpeed()
{
    ACharacter* AvatarChar = Cast<ACharacter>(GetAvatarActorFromActorInfo());
    if (AvatarChar)
    {
        AvatarChar->GetCharacterMovement()->MaxWalkSpeed = OriginalSpeed;
    }
}

void UImpunityAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    ResetSpeed();
    bIsActive = false;
    bIsOnCooldown = true;
    ACharacter* AvatarChar = Cast<ACharacter>(GetAvatarActorFromActorInfo());
    if (AvatarChar)
    {
        AvatarChar->GetWorldTimerManager().SetTimer(CooldownTimer, [this]() { bIsOnCooldown = false; }, CooldownDuration, false);
    }
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
