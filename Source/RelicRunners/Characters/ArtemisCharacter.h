#pragma once

#include "CoreMinimal.h"
#include "RelicRunners/RelicRunnersCharacter.h" 
#include "ArtemisCharacter.generated.h"

/**
 *
 */

UCLASS()
class RELICRUNNERS_API AArtemisCharacter : public ARelicRunnersCharacter
{
    GENERATED_BODY()

public:
    AArtemisCharacter();

protected:
    virtual void BeginPlay() override;
    
    virtual void GiveInitialAbilities() override;
};
