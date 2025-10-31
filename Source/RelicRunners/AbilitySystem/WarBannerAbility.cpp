// Fill out your copyright notice in the Description page of Project Settings.


#include "WarBannerAbility.h"
#include "WarBanner.h"
#include "Net/UnrealNetwork.h"
#include "../RelicRunnersCharacter.h"

AWarBannerAbility::AWarBannerAbility()
{
    bIsOnCooldown = false;
    bCanBeInterrupted = false;
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

    BannerSilhouetteMesh = CreateDefaultSubobject<UStaticMeshComponent>("BannerSilhoetteMesh");
    RootComponent = BannerSilhouetteMesh;

    SetReplicates(true);
}

void AWarBannerAbility::Server_Initialize_Implementation(AActor* newOwner)
{
    OwnerActor = newOwner;

    if (BannerSilhouetteMesh == nullptr)
        return;

    BannerSilhouetteMesh->SetVisibility(false);

    WarBanner = GetWorld()->SpawnActor<AWarBanner>(WarBannerTemplate, GetActorLocation(), GetActorRotation());
    WarBanner->Initialize(this);
}

void AWarBannerAbility::ActivateAbility()
{
    if (CanActivate() && OwnerActor != nullptr && WarBannerTemplate != nullptr)
    {
        if (BannerSilhouetteMesh == nullptr)
            return;

        BannerSilhouetteMesh->SetVisibility(true);
    }
    //Transparent version of a banner appears to indicate where it will spawn when the player presses left click button when started
    //Position of the banner is a line trace from the position of the camera to a certain distance away from the player

    //When you click the left mouse button when the transparent version is visible, Startup delay before the banner itself spawns
    //When the delay is complete, Spawn the banner exactly where it was shown.

    //Banner lasts 20 seconds | Radius is 100 meters | Banner can take damage and decreases lifespan when damaged | 
    //When the banner's life has ended tells the ability to start decreasing it's own cooldown timer

    //Order of operation
    /*
    Press ability input
    Aim the banner
    Press left mouse button
    Wait for the initial delay to end
    Spawn the actual banner
    Banner's life decreases overtime | If the banner takes damage, increase rate at which life decreases | Players in banner radius get a stat bonus
    Banner dies and tells ability to begin it's own cooldown timer
    */

    //Upgrades increase spawn distance + Banner Life + Banner Radius
}

void AWarBannerAbility::EndAbility()
{
    ARelicRunnersCharacter* character = Cast<ARelicRunnersCharacter>(OwnerActor);

    if (character == nullptr)
        return;

    bIsOnCooldown = false;
}

bool AWarBannerAbility::CanActivate() const
{
	return !bIsOnCooldown;
}

FName AWarBannerAbility::GetAbilityName() const
{
	return AbilityName;
}

void AWarBannerAbility::CancelAbility()
{
    if (BannerSilhouetteMesh == nullptr)
        return;

    BannerSilhouetteMesh->SetVisibility(false);
}

void AWarBannerAbility::Server_SpawnBanner_Implementation()
{
    bIsOnCooldown = true;

    if (WarBanner == nullptr)
        return;

    WarBanner->Server_Spawn(Duration, DamageAmount, AreaRadius);

    WarBanner->SetActorLocation(GetActorLocation());
    WarBanner->SetActorRotation(GetActorRotation());

    CancelAbility();
}
