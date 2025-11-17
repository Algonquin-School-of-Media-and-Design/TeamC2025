#include "EarthquakeAbility.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"

UEarthquakeAbility::UEarthquakeAbility()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    if (Duration <= 0.f) Duration = 8.0f;
    if (CooldownDuration <= 0.f) CooldownDuration = 10.0f;

    
}

void UEarthquakeAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    const FGameplayAbilityActorInfo* CurrentInfo = GetCurrentActorInfo();
    if (!CurrentInfo || !CurrentInfo->AvatarActor.IsValid())
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    AActor* AvatarActor = CurrentInfo->AvatarActor.Get();
    if (!AvatarActor)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    CachedActivationInfo = ActivationInfo;

    bIsActive = true;

    DrawDebugSphere(AvatarActor->GetWorld(), AvatarActor->GetActorLocation(), Radius, 32, FColor::Orange, false, Duration);

    // Tick timer
    AvatarActor->GetWorldTimerManager().SetTimer(TickTimerHandle, this, &UEarthquakeAbility::OnTick, TickRate, true);

    // Duration timer
    AvatarActor->GetWorldTimerManager().SetTimer(DurationTimerHandle, this, &UEarthquakeAbility::EndAbilityTimerCallback, Duration, false);
}

void UEarthquakeAbility::OnTick()
{
    const FGameplayAbilityActorInfo* Info = GetCurrentActorInfo();
    if (!Info || !Info->AvatarActor.IsValid())
    {
        EndAbility(GetCurrentAbilitySpecHandle(), Info, GetCurrentActivationInfo(), true, true);
        return;
    }

    AActor* AvatarActor = Info->AvatarActor.Get();
    if (!AvatarActor) return;

    ApplyDamageAndStun();

    DrawDebugSphere(AvatarActor->GetWorld(), AvatarActor->GetActorLocation(), Radius, 32, FColor::Orange, false, TickRate + 0.1f);
}

void UEarthquakeAbility::EndAbilityTimerCallback()
{
    EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), CachedActivationInfo, true, false);
}

void UEarthquakeAbility::ApplyDamageAndStun()
{
    AActor* AvatarActor = GetAvatarActorFromActorInfo();
    if (!AvatarActor) return;

    FVector Origin = AvatarActor->GetActorLocation();
    UWorld* World = AvatarActor->GetWorld();
    if (!World) return;

    TArray<AActor*> OverlappingActors;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

    UKismetSystemLibrary::SphereOverlapActors(World, Origin, Radius, ObjectTypes, ACharacter::StaticClass(), { AvatarActor }, OverlappingActors);

    for (AActor* Actor : OverlappingActors)
    {
        if (!Actor) continue;

        UGameplayStatics::ApplyDamage(Actor, DamagePerTick, AvatarActor->GetInstigatorController(), AvatarActor, nullptr);

        if (ACharacter* HitChar = Cast<ACharacter>(Actor))
        {
            FVector KnockDir = (HitChar->GetActorLocation() - Origin).GetSafeNormal();
            FVector Knockback = KnockDir * 250.f;
            Knockback.Z = 300.f;
            HitChar->LaunchCharacter(Knockback, true, true);
        }
    }
}

void UEarthquakeAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    if (ActorInfo && ActorInfo->AvatarActor.IsValid())
    {
        AActor* AvatarActor = ActorInfo->AvatarActor.Get();
        AvatarActor->GetWorldTimerManager().ClearTimer(TickTimerHandle);
        AvatarActor->GetWorldTimerManager().ClearTimer(DurationTimerHandle);
    }

    bIsActive = false;
    bIsOnCooldown = true;

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
