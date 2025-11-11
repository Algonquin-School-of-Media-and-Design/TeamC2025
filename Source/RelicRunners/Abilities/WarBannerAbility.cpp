#include "WarBannerAbility.h"
#include "WarBanner.h"
#include "Net/UnrealNetwork.h"
#include "../RelicRunnersCharacter.h"

UWarBannerAbility::UWarBannerAbility()
{
   /* bIsOnCooldown = false;
    Cooldown = 0.f;
    Duration = 10.f;
    DamageAmount = 0.f;
    AreaRadius = 500.f;
    ConeAngle = 0.f;
    AbilityName = "War Banner";
    AbilityCategory = EAbilityCategory::Utility;
    TargetType = EAbilityTargetType::Cone;
    OwnerActor = nullptr;
    BannerSilhouetteMesh = nullptr;
    WarBannerTemplate = nullptr;
    WarBanner = nullptr;

    BannerSilhouetteMesh = CreateDefaultSubobject<UStaticMeshComponent>("BannerSilhouetteMesh");
    RootComponent = BannerSilhouetteMesh;

    SetReplicates(true);*/
}

//void WarBannerAbility::Server_Initialize_Implementation(AActor* newOwner)
//{
//    OwnerActor = newOwner;
//
//    if (BannerSilhouetteMesh == nullptr)
//        return;
//
//    BannerSilhouetteMesh->SetVisibility(false);
//
//    if (WarBannerTemplate != nullptr && GetWorld())
//    {
//        WarBanner = GetWorld()->SpawnActor<AWarBanner>(WarBannerTemplate, GetActorLocation(), GetActorRotation());
//        if (WarBanner)
//        {
//            WarBanner->Initialize(this);
//        }
//    }
//}

void UWarBannerAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
   /* if (!CanActivate() || OwnerActor == nullptr || WarBannerTemplate == nullptr)
        return;

    if (BannerSilhouetteMesh == nullptr)
        return;

    BannerSilhouetteMesh->SetVisibility(true);*/

    /*
    Transparent version of a banner appears to indicate where it will spawn when the player presses left click button when started
    Position of the banner is a line trace from the position of the camera to a certain distance away from the player

    When you click the left mouse button when the transparent version is visible, Startup delay before the banner itself spawns
    When the delay is complete, Spawn the banner exactly where it was shown.

    Banner lasts 20 seconds | Radius is 100 meters | Banner can take damage and decreases lifespan when damaged | Players in banner radius get a stat bonus
    When the banner's life has ended tells the ability to start decreasing it's own cooldown timer

    Order of operation
    Press ability input
    Aim the banner
    Press left mouse button
    Wait for the initial delay to end
    Spawn the actual banner
    Banner's life decreases overtime | If the banner takes damage, increase rate at which life decreases | Players in banner radius get a stat bonus
    Banner dies and tells ability to begin it's own cooldown timer

    Upgrades increase spawn distance + Banner Life + Banner Radius
    */
}

void UWarBannerAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    /*ARelicRunnersCharacter* character = Cast<ARelicRunnersCharacter>(OwnerActor);
    if (!character)
        return;

    bIsActive = false;
    bIsOnCooldown = false;*/
}

//void WarBannerAbility::CancelAbility()
//{
//    if (BannerSilhouetteMesh)
//        BannerSilhouetteMesh->SetVisibility(false);
//}

void UWarBannerAbility::SpawnBanner(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
    /*if (!WarBanner)
        return;

    bIsOnCooldown = true;

    WarBanner->Server_Spawn(Duration, DamageAmount, AreaRadius);
    WarBanner->SetActorLocation(GetActorLocation());
    WarBanner->SetActorRotation(GetActorRotation());

    CancelAbility();*/
}



