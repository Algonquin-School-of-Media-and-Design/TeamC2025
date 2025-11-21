// Fill out your copyright notice in the Description page of Project Settings.


#include "ClassInfo.h"

FClassData UClassInfo::GetMaxStats() const
{
	FClassData MaxValues;
	MaxValues.Health = 0.f;
	MaxValues.Armor = 0.f;
	MaxValues.Dexterity = 0.f;
	MaxValues.Strength = 0.f;
	MaxValues.Intelligence = 0.f;

	for (const auto& Pair : Classes)
	{
		const FClassData& Info = Pair.Value;

		MaxValues.Health = FMath::Max(MaxValues.Health, Info.Health);
		MaxValues.Armor = FMath::Max(MaxValues.Armor, Info.Armor);
		MaxValues.Dexterity = FMath::Max(MaxValues.Dexterity, Info.Dexterity);
		MaxValues.Strength = FMath::Max(MaxValues.Strength, Info.Strength);
		MaxValues.Intelligence = FMath::Max(MaxValues.Intelligence, Info.Intelligence);
	}

	return MaxValues;
}