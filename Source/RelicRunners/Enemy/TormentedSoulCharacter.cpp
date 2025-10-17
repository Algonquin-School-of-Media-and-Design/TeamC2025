// Fill out your copyright notice in the Description page of Project Settings.


#include "TormentedSoulCharacter.h"
#include <RelicRunners/Item/ItemActor.h>
#include "RelicRunners/RelicRunnersCharacter.h"
#include <Net/UnrealNetwork.h>
#include "Components/WidgetComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SoldierController.h"

ATormentedSoulCharacter::ATormentedSoulCharacter() : Super()
{
	AttackOverlapCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Attack Collider"));
	AttackOverlapCollider->BodyInstance.SetCollisionProfileName("OverlapAll");
	AttackOverlapCollider->SetupAttachment(RootComponent);

	AIControllerClass = ASoldierController::StaticClass();
}

void ATormentedSoulCharacter::PrimaryAttack()
{
	IsAttacking = true;
	FTimerHandle timerHandle;
	float timeOfAnimation = 2.f;
	ATormentedSoulCharacter* soul = this;

	GetWorld()->GetTimerManager().SetTimer(timerHandle, [soul] {soul->IsAttacking = false; }, timeOfAnimation, false);
}

void ATormentedSoulCharacter::Die(AController* EventInstigator)
{
	if (HasAuthority())
	{
		if (EventInstigator)
		{
			if (ARelicRunnersCharacter* player = Cast<ARelicRunnersCharacter>(EventInstigator->GetPawn()))
			{
				player->AddExperience(CalculateXP());
			}
		}

		if (EnemyHUDWorld)
		{
			EnemyHUDWorld->DestroyComponent();
		}

		Destroy();
	}
}

float ATormentedSoulCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float actualDamage = DamageAmount;

	if (!EventInstigator->Tags.Contains("Player") && !IsAttacking)
	{
		actualDamage *= 0.5;
	}

	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ATormentedSoulCharacter::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsAttacking && OtherActor->Tags.Contains("Player"))
	{
		UGameplayStatics::ApplyDamage(OtherActor, AttackDamage, this->Controller, this, nullptr);
	}
}
