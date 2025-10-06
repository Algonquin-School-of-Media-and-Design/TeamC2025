// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySelection.h"
#include "Components/Button.h"
#include "RelicRunners/RelicRunnersCharacter.h"


void UAbilitySelection::NativeConstruct()
{
    Super::NativeConstruct();

    B_Upgrade1->OnClicked.AddDynamic(this, &UAbilitySelection::Upgrade1Clicked);
    B_Upgrade2->OnClicked.AddDynamic(this, &UAbilitySelection::Upgrade2Clicked);
    B_Upgrade3->OnClicked.AddDynamic(this, &UAbilitySelection::Upgrade3Clicked);
    B_Upgrade4->OnClicked.AddDynamic(this, &UAbilitySelection::Upgrade4Clicked);

    APlayerController* PC = GetOwningPlayer();
    PC = GetOwningPlayer();
    if (PC)
    {
        APawn* Pawn = PC->GetPawn();
        OwningCharacter = Cast<ARelicRunnersCharacter>(Pawn);
    }

}

void UAbilitySelection::SpendAbilityPoints()
{
    int CurrentAbilityPoints = OwningCharacter->GetPlayerAbilityPoints();

    if (CurrentAbilityPoints >= 1)
    {
        CurrentAbilityPoints--;
        OwningCharacter->UpdateHUD();
    }

}

void UAbilitySelection::Upgrade1Clicked()
{
    SpendAbilityPoints();
}

void UAbilitySelection::Upgrade2Clicked()
{
    SpendAbilityPoints();
}

void UAbilitySelection::Upgrade3Clicked()
{
    SpendAbilityPoints();
}

void UAbilitySelection::Upgrade4Clicked()
{
    SpendAbilityPoints();
}

