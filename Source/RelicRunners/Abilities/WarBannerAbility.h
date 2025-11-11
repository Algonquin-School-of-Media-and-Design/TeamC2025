#pragma once

#include "CoreMinimal.h"
#include "RelicRunners/AbilitySystem/AbilityBase.h"
#include "WarBannerAbility.generated.h"

UCLASS()
class RELICRUNNERS_API UWarBannerAbility : public UAbilityBase
{
    GENERATED_BODY()

public:
    UWarBannerAbility();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
    virtual FName GetAbilityName() const override { return FName("War Banner"); }

protected:
    //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WarBanner")
    //TSubclassOf<class AWarBanner> WarBannerTemplate;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "WarBanner")
    class UStaticMeshComponent* BannerSilhouetteMesh;

private:
    FTimerHandle BannerDurationTimer;

    //UPROPERTY()
    //class AWarBanner* WarBanner;

    void SpawnBanner(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo);
};
