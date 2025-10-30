#include "BundleOfJoy.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"

ABundleOfJoy::ABundleOfJoy()
{
    PrimaryActorTick.bCanEverTick = true;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    AttractionRadius = 600.f;
    ExplosionRadius = 400.f;
    AttractionStrength = 1500.f;
    DamageAmount = 100.f;
    Duration = 3.f;
}

void ABundleOfJoy::BeginPlay()
{
    Super::BeginPlay();
}

void ABundleOfJoy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsActive)
    {
        FVector Center = GetActorLocation();

        DrawDebugSphere(GetWorld(), Center, 50.f, 16, FColor::Red, false, -1.f, 0, 5.f);
        DrawDebugCircle(GetWorld(), Center, AttractionRadius, 32, FColor::Purple, false, -1.f, 0, 5.f, FVector(1, 0, 0), FVector(0, 1, 0), false);

        Attract();
    }
}

bool ABundleOfJoy::CanActivate() const
{
    return !bIsOnCooldown && !bIsActive;
}

void ABundleOfJoy::ActivateAbility()
{
    if (!CanActivate())
        return;

    bIsActive = true;

    GetWorld()->GetTimerManager().SetTimer(ExplosionTimer, this, &ABundleOfJoy::Explode, Duration, false);
}

void ABundleOfJoy::Attract()
{
    if (!bIsActive) return;

    FVector Center = GetActorLocation();
    UWorld* World = GetWorld();
    if (!World) return;

    TArray<AActor*> OverlappingActors;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

    UKismetSystemLibrary::SphereOverlapActors(World, Center, AttractionRadius, ObjectTypes, ACharacter::StaticClass(), { OwnerActor }, OverlappingActors);

    for (AActor* Actor : OverlappingActors)
    {
        if (!Actor) continue;

        if (ACharacter* Char = Cast<ACharacter>(Actor))
        {
            FVector Dir = (Center - Char->GetActorLocation());
            Dir.Z = 0.f; 
            Dir.Normalize();

            Char->AddMovementInput(Dir, AttractionStrength * World->GetDeltaSeconds());

            FRotator TargetRot = Dir.Rotation();
            TargetRot.Pitch = 0.f; 
            TargetRot.Roll = 0.f;
            Char->SetActorRotation(TargetRot);
        }

    }
}


void ABundleOfJoy::Explode()
{
    if (!bIsActive) return;

    bIsActive = false;
    bIsOnCooldown = true;

    FVector Center = GetActorLocation();
    UWorld* World = GetWorld();
    if (!World) return;

    TArray<AActor*> OverlappingActors;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

    UKismetSystemLibrary::SphereOverlapActors(World, Center, ExplosionRadius, ObjectTypes, ACharacter::StaticClass(), { OwnerActor }, OverlappingActors);

    for (AActor* Actor : OverlappingActors)
    {
        if (!Actor) continue;
        UGameplayStatics::ApplyDamage(Actor, DamageAmount, nullptr, this, nullptr);
    }

    DrawDebugSphere(World, Center, ExplosionRadius, 32, FColor::Red, false, 2.f);

    GetWorld()->GetTimerManager().SetTimer(CooldownTimer, [this]() { bIsOnCooldown = false; }, 5.f, false);

    Destroy();
}

void ABundleOfJoy::EndAbility()
{
    Explode();
}
