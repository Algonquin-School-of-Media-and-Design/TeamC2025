#include "Moonbeam.h"

#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

#include "RelicRunners/Enemy/EnemyCharacter.h"
#include "RelicRunners/Enemy/EnemyCharacterAI.h"



AMoonbeam::AMoonbeam()
{
    // Identity
    AbilityName = FName(TEXT("Moonbeam"));
    AbilityCategory = EAbilityCategory::Offensive;
    TargetType = EAbilityTargetType::Cone; 

    // Tuning
    DamageAmount = 20.f;      
    Cooldown = 5.f;       
    Duration = 5.0f;       

    // Beam visuals
    BeamRange = 1500.f;
    BeamRadius = 6.f;
    BeamDuration = 5.0f;

    bIsOnCooldown = false;
    bCanBeInterrupted = true;
}

bool AMoonbeam::CanActivate() const
{
    return !bIsOnCooldown;
}

void AMoonbeam::ActivateAbility()
{
    if (!GetWorld())
    {
        return;
    }

    if (!CanActivate())
    {
        UE_LOG(LogTemp, Verbose, TEXT("Moonbeam: Tried to cast while on cooldown."));
        return;
    }

    
    AActor* LocalOwner = OwnerActor ? OwnerActor : GetOwner();
    ACharacter* OwnerChar = Cast<ACharacter>(LocalOwner);

    if (!OwnerChar)
    {
        UE_LOG(LogTemp, Warning, TEXT("Moonbeam: No valid character owner."));
        return;
    }

    FVector Start;
    FRotator ViewRot;

    if (OwnerChar->GetController())
    {
        OwnerChar->GetController()->GetPlayerViewPoint(Start, ViewRot);
    }
    else
    {
        OwnerChar->GetActorEyesViewPoint(Start, ViewRot);
    }

    const FVector Dir = ViewRot.Vector();
    const FVector End = Start + Dir * BeamRange;

    SpawnBeamEffect(Start, End);

    // DAMAGE (server authoritative, piercing)
    if (OwnerChar->HasAuthority())
    {
        FCollisionQueryParams Params(SCENE_QUERY_STAT(MoonbeamTrace), false);
        Params.AddIgnoredActor(OwnerChar);
        Params.AddIgnoredActor(this);

        TSet<AActor*> HitActors;

        const FVector TraceDir = (End - Start).GetSafeNormal();
        FVector TraceStart = Start;
        float RemainingDistance = BeamRange;
        const float Epsilon = 10.f; // step past each hit so we don't get stuck

        while (RemainingDistance > 0.f)
        {
            FHitResult Hit;
            const FVector TraceEnd = TraceStart + TraceDir * RemainingDistance;

            if (!GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Pawn, Params))
            {
                break; // no more hits along the rest of the beam
            }

            AActor* HitActor = Hit.GetActor();
            if (!HitActor)
            {
                break;
            }

            // Only damage your enemies
            if (Cast<AEnemyCharacterAI>(HitActor) || Cast<AEnemyCharacter>(HitActor))
            {
                if (!HitActors.Contains(HitActor))
                {
                    HitActors.Add(HitActor);

                    UGameplayStatics::ApplyDamage(
                        HitActor,
                        DamageAmount,
                        OwnerChar->GetController(),
                        OwnerChar,   // DamageCauser
                        nullptr
                    );

                    UE_LOG(LogTemp, Log, TEXT("Moonbeam pierced %s for %f damage"),
                        *HitActor->GetName(), DamageAmount);
                }
            }

            Params.AddIgnoredActor(HitActor);

            // Advance start a bit past this hit
            const float Traveled = (Hit.ImpactPoint - TraceStart).Size();
            RemainingDistance -= (Traveled + Epsilon);
            TraceStart = Hit.ImpactPoint + TraceDir * Epsilon;
        }
    }

    // === COOLDOWN ===
    bIsOnCooldown = true;
    if (Cooldown > 0.f)
    {
        GetWorld()->GetTimerManager().SetTimer(
            CooldownTimerHandle,
            this,
            &AMoonbeam::ResetCooldown,
            Cooldown,
            false
        );
    }
}

void AMoonbeam::EndAbility()
{
    // Not used yet.
}

void AMoonbeam::ResetCooldown()
{
    bIsOnCooldown = false;
}

void AMoonbeam::SpawnBeamEffect(const FVector& Start, const FVector& End)
{
    if (!GetWorld())
    {
        return;
    }

    DrawDebugLine(
        GetWorld(),
        Start,
        End,
        FColor::Cyan,
        false,
        BeamDuration,
        0,
        BeamRadius
    );
}
