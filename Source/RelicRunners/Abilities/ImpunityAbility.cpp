// Fill out your copyright notice in the Description page of Project Settings.


#include "ImpunityAbility.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"

AImpunityAbility::AImpunityAbility()
{
    Duration = 5.0f;
    Cooldown = 8.0f;

    PrimaryActorTick.bCanEverTick = false;
}

void AImpunityAbility::BeginPlay()
{
    Super::BeginPlay();
}

bool AImpunityAbility::CanActivate() const
{
    return !bIsOnCooldown && !bIsActive;
}

void AImpunityAbility::ActivateAbility()
{
    if (!CanActivate() || !OwnerActor)
        return;

    ACharacter* PlayerChar = Cast<ACharacter>(OwnerActor);
    if (!PlayerChar)
        return;

    UCharacterMovementComponent* MoveComp = PlayerChar->GetCharacterMovement();
    if (!MoveComp)
        return;

    bIsActive = true;

    OriginalSpeed = MoveComp->MaxWalkSpeed;
    MoveComp->MaxWalkSpeed *= 1.75f;  // Increase speed by 75%

    GetWorld()->GetTimerManager().SetTimer(SpeedResetTimer, this, &AImpunityAbility::ResetSpeed, Duration, false);
}

void AImpunityAbility::ResetSpeed()
{
    if (!OwnerActor)
        return;

    ACharacter* PlayerChar = Cast<ACharacter>(OwnerActor);
    if (PlayerChar)
    {
        UCharacterMovementComponent* MoveComp = PlayerChar->GetCharacterMovement();
        if (MoveComp)
        {
            MoveComp->MaxWalkSpeed = OriginalSpeed;
        }
    }

    EndAbility();
}

void AImpunityAbility::EndAbility()
{
    Super::EndAbility();

    bIsActive = false;
    bIsOnCooldown = true;

    GetWorld()->GetTimerManager().SetTimer(CooldownTimer, [this]()
        {
            bIsOnCooldown = false;
        }, Cooldown, false);
}