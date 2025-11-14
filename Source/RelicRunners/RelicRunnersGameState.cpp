// Fill out your copyright notice in the Description page of Project Settings.


#include "RelicRunnersGameState.h"
#include <Net/UnrealNetwork.h>

ARelicRunnersGameState::ARelicRunnersGameState()
{
    bReplicates = true;
}

bool ARelicRunnersGameState::InitializeTriggerState()
{
    if (ObjectiveType == 0)
    {
        return true;
    }
    return false;
}

void ARelicRunnersGameState::Multicast_SetObjectiveType_Implementation(EObjectiveType newType)
{
    if (HasAuthority())
    {
        ObjectiveType = static_cast<uint8>(newType);

        if (ObjectiveType == 0)
        {
            Multicast_DecrementObjective();
        }
    }
}

void ARelicRunnersGameState::Multicast_SetObjectiveType(int newType)
{
    if (HasAuthority())
    {
        ObjectiveType = newType;

        if (ObjectiveType == 0)
        {
            Multicast_DecrementObjective();
        }
    }
}

void ARelicRunnersGameState::Multicast_IncrementObjective_Implementation()
{
    if (HasAuthority())
    {
        RemainingObjectives++;
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("Max objectives: %i"), RemainingObjectives));
    }
}

void ARelicRunnersGameState::Multicast_DecrementObjective_Implementation()
{
    RemainingObjectives--;

    if (RemainingObjectives <= 0)
    {
        OnObjectiveActionCompleted.Broadcast();
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