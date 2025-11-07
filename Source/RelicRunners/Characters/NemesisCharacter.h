#pragma once

#include "CoreMinimal.h"
#include "RelicRunners/RelicRunnersCharacter.h" 
#include "NemesisCharacter.generated.h"

/**
 *
 */

UCLASS()
class RELICRUNNERS_API ANemesisCharacter : public ARelicRunnersCharacter
{
    GENERATED_BODY()

public:
    ANemesisCharacter();

    virtual void PossessedBy(AController* NewController) override;
    virtual void OnRep_PlayerState() override;

protected:
    virtual void BeginPlay() override;

    virtual void GiveDamageAbilities() override;
    virtual void GiveDefenceAbilities() override;
    virtual void GiveUtilityAbilities() override;
    virtual void GiveUltimateAbilities() override;

};
