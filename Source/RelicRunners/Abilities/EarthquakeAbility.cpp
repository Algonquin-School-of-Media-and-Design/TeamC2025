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
    if (Cooldown <= 0.f) Cooldown = 10.0f;
}

void UEarthquakeAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    AActor* AvatarActor = GetAvatarActorFromActorInfo();
    if (!AvatarActor) return;

    bIsActive = true;

    DrawDebugSphere(AvatarActor->GetWorld(), AvatarActor->GetActorLocation(), Radius, 32, FColor::Orange, false, Duration);

    AvatarActor->GetWorldTimerManager().SetTimer(TickTimerHandle, [this, AvatarActor](){ApplyDamageAndStun();DrawDebugSphere(AvatarActor->GetWorld(), AvatarActor->GetActorLocation(), Radius, 32, FColor::Orange, false, TickRate + 0.1f);}, TickRate, true);

    AvatarActor->GetWorldTimerManager().SetTimer(DurationTimerHandle, FTimerDelegate::CreateUObject(this, &UEarthquakeAbility::EndAbility, Handle, ActorInfo, ActivationInfo, true, false), Duration, false);
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
    AActor* AvatarActor = GetAvatarActorFromActorInfo();
    if (AvatarActor)
    {
        AvatarActor->GetWorldTimerManager().ClearTimer(TickTimerHandle);
        AvatarActor->GetWorldTimerManager().ClearTimer(DurationTimerHandle);
    }

    bIsActive = false;
    bIsOnCooldown = true;

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
