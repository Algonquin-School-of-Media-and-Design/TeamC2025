#include "VengefulDance.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"

UVengefulDance::UVengefulDance()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    AbilityName = FName("VengefulDance");

    DamageInterval = 0.5f;
    KnockbackStrength = 1800.f;
    Duration = 1.f;
    DamageAmount = 25.f;
    AreaRadius = 300.f;

    CooldownDuration = 5.f;
    CooldownTag = FGameplayTag::RequestGameplayTag("Gameplay.Cooldown.VengefulDance");

    CooldownTagContainer.AddTag(CooldownTag);

    ActivationBlockedTags.AddTag(CooldownTag);
}

void UVengefulDance::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{

    UE_LOG(LogTemp, Warning, TEXT("[VengefulDance] ActivateAbility called"));

    if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("[VengefulDance] Invalid ActorInfo or ASC!"));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();

    // Check if ability is on cooldown
    if (ASC->HasMatchingGameplayTag(CooldownTag))
    {
        UE_LOG(LogTemp, Warning, TEXT("[VengefulDance] Ability is on cooldown!"));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        UE_LOG(LogTemp, Warning, TEXT("[VengefulDance] CommitAbility failed"));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // Apply the cooldown GE
    if (GenericCooldownEffect)
    {
        FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GenericCooldownEffect, GetAbilityLevel(), ASC->MakeEffectContext());
        if (SpecHandle.IsValid() && SpecHandle.Data.IsValid())
        {
            SpecHandle.Data->DynamicGrantedTags.AddTag(CooldownTag);
            SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.CooldownDuration"), CooldownDuration);
            ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
            UE_LOG(LogTemp, Warning, TEXT("[VengefulDance] Cooldown applied: %f seconds"), CooldownDuration);
        }
    }

    // Damage logic
    AActor* AvatarActor = GetAvatarActorFromActorInfo();
    if (!AvatarActor) return;

    UE_LOG(LogTemp, Warning, TEXT("[VengefulDance] Starting damage timers"));

    AvatarActor->GetWorldTimerManager().SetTimer(DamageTickTimer, [this]() { ApplyRingDamage(); }, DamageInterval, true);
    AvatarActor->GetWorldTimerManager().SetTimer(EndTimer,
        FTimerDelegate::CreateUObject(this, &UVengefulDance::EndAbility, Handle, ActorInfo, ActivationInfo, true, false),
        Duration, false);

    DrawDebugCircle(AvatarActor->GetWorld(), AvatarActor->GetActorLocation(), AreaRadius, 64, FColor::Red, false, Duration, 0, 5.f, FVector(1, 0, 0), FVector(0, 1, 0), false);

}

void UVengefulDance::ApplyRingDamage()
{
    AActor* AvatarActor = GetAvatarActorFromActorInfo(); // Get the actor that owns this ability
    if (!AvatarActor) return; // Exit if invalid

    UWorld* World = AvatarActor->GetWorld(); // Get the world
    if (!World) return; // Exit if world is invalid

    FVector Origin = AvatarActor->GetActorLocation(); // Center of the damage/knockback
    TArray<AActor*> OverlappingActors; // Array to store actors within the radius

    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; // Which kinds of objects the sphere check should detect
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn)); // Only pawns

    // Find all actors overlapping a sphere around the origin, ignoring the owner
    UKismetSystemLibrary::SphereOverlapActors(World, Origin, AreaRadius, ObjectTypes, ACharacter::StaticClass(), { AvatarActor }, OverlappingActors);

    for (AActor* Actor : OverlappingActors) // Loop through all overlapping actors
    {
        if (!Actor || Actor == AvatarActor) // Skip invalid actors or the owner
            continue;

        FVector Diff = Actor->GetActorLocation() - Origin; // Vector from origin to actor
        Diff.Z = 0.f; // Ignore vertical distance

        if (Diff.Size() > AreaRadius) // Skip actors outside the area radius
            continue;

        UGameplayStatics::ApplyDamage(Actor, DamageAmount, AvatarActor->GetInstigatorController(), AvatarActor, nullptr); // Apply damage

        if (ACharacter* HitCharacter = Cast<ACharacter>(Actor)) // If the actor is a character
        {
            FVector Knockback = Diff.GetSafeNormal() * KnockbackStrength; // Directional knockback
            Knockback.Z = 400.f; // Add upward force
            HitCharacter->LaunchCharacter(Knockback, true, false); // Apply knockback
        }
    }

}

void UVengefulDance::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    AActor* AvatarActor = GetAvatarActorFromActorInfo();
    if (AvatarActor)
    {
        //Clear timers
        AvatarActor->GetWorldTimerManager().ClearTimer(DamageTickTimer);
        AvatarActor->GetWorldTimerManager().ClearTimer(EndTimer);
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
