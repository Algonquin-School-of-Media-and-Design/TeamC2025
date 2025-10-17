#pragma once

#include "CoreMinimal.h"
#include "Engine/DamageEvents.h"
#include "Templates/SubclassOf.h"
#include "Engine/NetSerialization.h"
#include "CustomDamageEvents.generated.h"

class UDamageType;

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	LIGHT_ATTACK = 0   UMETA(DisplayName = "Light Attack"),
	HEAVY_ATTACK = 1   UMETA(DisplayName = "Melee"),
	UNKOWN = 2         UMETA(DisplayName = "UNKNOWN")
};

USTRUCT(BlueprintType)
struct RELICRUNNERS_API FAttackDamageEvent : public FDamageEvent
{
	GENERATED_BODY()

	public: 
		EAttackType AttackType;
		float StunDuration;
	
		FAttackDamageEvent() : FDamageEvent(), AttackType(EAttackType::UNKOWN), StunDuration(0.0f) { }

		FAttackDamageEvent(EAttackType attackType, float stunDuration, TSubclassOf<UDamageType> inDamageTypeClass) : FDamageEvent(inDamageTypeClass), AttackType(attackType), StunDuration(stunDuration) { }

		virtual ~FAttackDamageEvent() { }

		static const int32 ClassID = 3;

		virtual int32 GetTypeID() const { return FAttackDamageEvent::ClassID; }
		virtual bool IsOfType(int32 InID) const { return FAttackDamageEvent::ClassID == InID; };
};