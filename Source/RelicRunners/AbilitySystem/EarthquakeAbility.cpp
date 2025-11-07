// Fill out your copyright notice in the Description page of Project Settings.


#include "EarthquakeAbility.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"

AEarthquakeAbility::AEarthquakeAbility()
{
    PrimaryActorTick.bCanEverTick = true;

    // Duration and Cooldown
    if (Duration <= 0.f) Duration = 8.0f;
    if (Cooldown <= 0.f) Cooldown = 10.0f;
}


void AEarthquakeAbility::BeginPlay()
{
    Super::BeginPlay();
}


bool AEarthquakeAbility::CanActivate() const
{
    return !bIsOnCooldown && !bIsActive;
}


void AEarthquakeAbility::ActivateAbility()
{
    if (!CanActivate() || !OwnerActor) return;

    bIsActive = true;

    const float LifeTime = (Duration > 0.f) ? Duration : 8.0f;

    // so it follows the owner if they move.
    DrawDebugSphere(GetWorld(), OwnerActor->GetActorLocation(), Radius, 32, FColor::Orange, false, LifeTime);

    GetWorld()->GetTimerManager().SetTimer(TickTimerHandle, this, &AEarthquakeAbility::ApplyDamageAndStun, TickRate, true);
    GetWorld()->GetTimerManager().SetTimer(DurationTimerHandle, this, &AEarthquakeAbility::EndAbility, LifeTime, false);
}


void AEarthquakeAbility::ApplyDamageAndStun()
{
    FVector Origin = OwnerActor->GetActorLocation();

    TArray<AActor*> OverlappingActors;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));


    UKismetSystemLibrary::SphereOverlapActors(
        GetWorld(), Origin, Radius, ObjectTypes,
        ACharacter::StaticClass(), { OwnerActor }, OverlappingActors
    );


    for (AActor* Actor : OverlappingActors)
    {
        if (!Actor) continue;


        UGameplayStatics::ApplyDamage(Actor, DamagePerTick, nullptr, this, nullptr);


        if (ACharacter* HitChar = Cast<ACharacter>(Actor))
        {
            FVector KnockDir = (HitChar->GetActorLocation() - Origin).GetSafeNormal();
            FVector Knockback = KnockDir * 250.f; // low knockback for stun effect
            Knockback.Z = 300.f;
            HitChar->LaunchCharacter(Knockback, true, true);
        }
    }
}

void AEarthquakeAbility::EndAbility()
{
    Super::EndAbility();

    bIsActive = false;
    bIsOnCooldown = true;

    GetWorld()->GetTimerManager().ClearTimer(TickTimerHandle);

    GetWorld()->GetTimerManager().SetTimer(CooldownTimer, [this]() {
        bIsOnCooldown = false;
        }, Cooldown, false);
}


void AEarthquakeAbility::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Keep the debug sphere visible and follow the owner during the active duration.
    if (bIsActive && OwnerActor)
    {
        DrawDebugSphere(GetWorld(), OwnerActor->GetActorLocation(), Radius, 32, FColor::Orange, false, TickRate + 0.1f);
    }
}