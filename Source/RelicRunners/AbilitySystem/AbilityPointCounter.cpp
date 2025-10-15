// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityPointCounter.h"
#include "Components/TextBlock.h"

void UAbilityPointCounter::NativeConstruct()
{
	Super::NativeConstruct();

}


void UAbilityPointCounter::UpdateHUD(int AbilityPoints)
{
	TB_AbilityPoints->SetText(FText::FromString(FString::Printf(TEXT("%d"), AbilityPoints)));


}
