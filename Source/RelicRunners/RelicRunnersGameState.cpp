// Fill out your copyright notice in the Description page of Project Settings.


#include "RelicRunnersGameState.h"
#include <Net/UnrealNetwork.h>

ARelicRunnersGameState::ARelicRunnersGameState()
{
    bReplicates = true;
}

bool ARelicRunnersGameState::InitializeTriggerState()
{
    switch (ObjectiveType)
    {
    case EObjectiveType::None:
        return true;
        break;
    case EObjectiveType::CaptureTheFlag:
        return false;
        break;
    }
    return false;
}

void ARelicRunnersGameState::Multicast_SetObjectiveType_Implementation(EObjectiveType newType)
{
    if (HasAuthority())
    {
        ObjectiveType = newType;

        switch (ObjectiveType)
        {
        case EObjectiveType::None:
            Multicast_DecrementObjective();
            break;
        }
    }
}

void ARelicRunnersGameState::Multicast_IncrementObjective_Implementation()
{
    if (HasAuthority())
    {
        RemainingObjectives++;
    }
}

void ARelicRunnersGameState::Multicast_DecrementObjective_Implementation()
{
    RemainingObjectives--;

    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("Remaining objectives: %i"), RemainingObjectives));

    if (RemainingObjectives <= 0)
    {
        OnObjectiveActionCompleted.Broadcast();
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("Objective has been reached!")));
    }
}

void ARelicRunnersGameState::OnRep_ObjectivesChange()
{
    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("OnRep - Remaining objectives: %i"), RemainingObjectives));

    if (RemainingObjectives <= 0)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("OnRep - Objective has been reached!")));
    }
}

void ARelicRunnersGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ARelicRunnersGameState, RemainingObjectives);
    DOREPLIFETIME(ARelicRunnersGameState, ObjectiveType);
}