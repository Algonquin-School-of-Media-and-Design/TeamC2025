#include "VengefulDance.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"

AVengefulDance::AVengefulDance()
{
    DamageInterval = 0.5f;
    KnockbackStrength = 1800.0f;
    Duration = 1.0f;           
    Cooldown = 5.0f;
    DamageAmount = 25.0f;
    AreaRadius = 300.0f;

    PrimaryActorTick.bCanEverTick = true;
}

void AVengefulDance::BeginPlay()
{
    Super::BeginPlay();
}

bool AVengefulDance::CanActivate() const
{
    return !bIsOnCooldown && !bIsActive;
}

void AVengefulDance::ActivateAbility()
{
    if (!CanActivate())
        return;

    bIsActive = true;


    GetWorld()->GetTimerManager().SetTimer(DamageTickTimer, this, &AVengefulDance::ApplyRingDamage, DamageInterval, true);

    GetWorld()->GetTimerManager().SetTimer(EndTimer, this, &AVengefulDance::EndAbility, Duration, false);

    FVector Center = OwnerActor->GetActorLocation();
    float Radius = 300.f; 


    DrawDebugCircle(
        GetWorld(),
        Center,
        Radius,
        64,            
        FColor::Red,
        false,         
        2.f,           
        0,             
        5.f,           
        FVector(1, 0, 0),
        FVector(0, 1, 0),
        false          
    );
}

void AVengefulDance::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AVengefulDance::ApplyRingDamage()
{
    if (!OwnerActor) return;
    UWorld* World = GetWorld();
    if (!World) return;

    FVector Origin = OwnerActor->GetActorLocation();
    TArray<AActor*> OverlappingActors;


    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

    UKismetSystemLibrary::SphereOverlapActors(World, Origin, AreaRadius,ObjectTypes, ACharacter::StaticClass(),{ OwnerActor },OverlappingActors);


    for (AActor* Actor : OverlappingActors)
    {
        if (!Actor || Actor == OwnerActor)
            continue;

        FVector TargetLoc = Actor->GetActorLocation();
        FVector Diff = TargetLoc - Origin;


        Diff.Z = 0.0f;

        float DistanceXY = Diff.Size();
        if (DistanceXY > AreaRadius)
            continue; 


        UGameplayStatics::ApplyDamage(Actor, DamageAmount, OwnerActor->GetInstigatorController(), OwnerActor, nullptr);


        if (ACharacter* HitCharacter = Cast<ACharacter>(Actor))
        {
            FVector KnockbackDir = Diff.GetSafeNormal();
            FVector Knockback = KnockbackDir * KnockbackStrength;

            Knockback.Z = 400;

            HitCharacter->LaunchCharacter(Knockback, true, false);
        }

    }
}

void AVengefulDance::EndAbility()
{
    Super::EndAbility();

    bIsActive = false;
    bIsOnCooldown = true;

    GetWorld()->GetTimerManager().ClearTimer(DamageTickTimer);

    GetWorld()->GetTimerManager().SetTimer(CooldownTimer, [this]() { bIsOnCooldown = false; }, Cooldown, false);
}
