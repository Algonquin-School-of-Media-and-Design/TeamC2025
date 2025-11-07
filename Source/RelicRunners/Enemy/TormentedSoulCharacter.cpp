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
	AttackOverlapCollider->SetupAttachment(GetMesh());

	AIControllerClass = ASoldierController::StaticClass();
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

	//ask Anto if I can fix his applay damage call because he is not assigning the EventInstigator properly
	if (EventInstigator && !EventInstigator->Tags.Contains("Player") && !IsAttacking)
	{
		actualDamage *= 0.5;
	}

	return Super::TakeDamage(actualDamage, DamageEvent, EventInstigator, DamageCauser);
}

void ATormentedSoulCharacter::BasicAttack(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Log, TEXT("should damage"));
	if (IsAttacking && OtherActor->Tags.Contains("Player"))
	{
		UGameplayStatics::ApplyDamage(OtherActor, AttackDamage, this->Controller, this, nullptr);
	}
}
