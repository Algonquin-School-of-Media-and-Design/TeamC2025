#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AbilityInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UAbilityInterface : public UInterface
{
    GENERATED_BODY()
};

class IAbilityInterface
{
    GENERATED_BODY()

public:
    virtual void ActivateAbility() = 0;
    virtual void EndAbility() = 0;
    virtual bool CanActivate() const = 0;
    virtual FName GetAbilityName() const = 0;
};
