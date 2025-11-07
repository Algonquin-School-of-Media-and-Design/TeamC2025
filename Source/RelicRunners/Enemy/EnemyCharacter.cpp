// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "RelicRunners/CustomDamageEvents.h"
#include "RelicRunners/Spawning System/Director.h"
#include <Kismet/GameplayStatics.h>
#include <RelicRunners/Item/ItemActor.h>
#include "RelicRunners/RelicRunnersCharacter.h"
#include "Components/WidgetComponent.h"
#include <Net/UnrealNetwork.h>


// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CurrentHealth = 100;
	MaxHealth = 100;
	RemainingStunTime = 0;
	Level = 1;
	TypeOfEnemy = EEnemyType::UNKNOWN;

	//tristan UI stuff
	EnemyHUDWorld = CreateDefaultSubobject<UWidgetComponent>(TEXT("TooltipWidgetComponent"));
	EnemyHUDWorld->SetupAttachment(GetMesh());
	EnemyHUDWorld->SetWorldScale3D(FVector(0.1, 0.1, 0.1));
	EnemyHUDWorld->SetRelativeLocation(FVector(0.f, 0.f, 200.f));
	EnemyHUDWorld->SetWidgetSpace(EWidgetSpace::World);
	EnemyHUDWorld->SetDrawAtDesiredSize(true);
	EnemyHUDWorld->SetVisibility(true);
	EnemyHUDWorld->CastShadow = false;

	bReplicates = true;
	SetReplicateMovement(true);
}

void AEnemyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEnemyCharacter, CurrentHealth);
	DOREPLIFETIME(AEnemyCharacter, MaxHealth);
	DOREPLIFETIME(AEnemyCharacter, RemainingStunTime);
	DOREPLIFETIME(AEnemyCharacter, Level);
	DOREPLIFETIME(AEnemyCharacter, TypeOfEnemy);
	DOREPLIFETIME(AEnemyCharacter, EnemyHUDWorld);
}

float AEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	CurrentHealth -= DamageAmount;

	if (DamageAmount < 0)
	{
		DamageAmount = 0;
	}

	if (IsDead())
	{
		Die(EventInstigator);
	}
	else
	{
		if (DamageEvent.IsOfType(FAttackDamageEvent::ClassID))
		{
			FAttackDamageEvent* AttackDamageEvent = (FAttackDamageEvent*)&DamageEvent;
			if (AttackDamageEvent && (
				//light enemy stunned by light or heavy attack
				(AttackDamageEvent->AttackType == EAttackType::LIGHT_ATTACK || AttackDamageEvent->AttackType == EAttackType::HEAVY_ATTACK) && TypeOfEnemy == EEnemyType::LIGHT || 
				//medium enemy stunned by heavy attack
				AttackDamageEvent->AttackType == EAttackType::HEAVY_ATTACK && TypeOfEnemy == EEnemyType::MEDIUM))
			{
				RemainingStunTime = AttackDamageEvent->StunDuration;
			}
		}
	}

	return DamageAmount;
}

void AEnemyCharacter::Die(AController* EventInstigator)
{
	if (HasAuthority())
	{
		SpawnItem();

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

void AEnemyCharacter::ReduceStunTime(float DeltaTime)
{
	if (RemainingStunTime > 0)
	{
		RemainingStunTime -= DeltaTime;
		if (RemainingStunTime < 0)
		{
			RemainingStunTime = 0;
		}
	}
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;

	if (HasAuthority())
	{
		UWorld* World = GetWorld();
		APawn* enemy = static_cast<APawn*>(this);

		GetWorld()->GetTimerManager().SetTimerForNextTick([World, enemy] {
			ADirector* Director = static_cast<ADirector*>(UGameplayStatics::GetActorOfClass(World, ADirector::StaticClass()));

			if (Director != nullptr)
			{
				Director->AddEnemy(enemy);
				enemy->OnDestroyed.AddDynamic(Director, &ADirector::RemoveEnemy);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("missing director"));
			}
		});
	}
}

void AEnemyCharacter::SpawnItem()
{
	if (ItemLootPool.Num() > 0)
	{
		int randomIndex = FMath::RandRange(0, ItemLootPool.Num() - 1);
		UItemObject* itemToSpawn = ItemLootPool[randomIndex];

		if (itemToSpawn != nullptr)
		{
			FVector spawnLocation = GetActorLocation();
			FRotator spawnRotation = FRotator::ZeroRotator;
			FActorSpawnParameters spawnParams;
			spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			AItemActor* spawnedItem = GetWorld()->SpawnActor<AItemActor>(AItemActor::StaticClass(), spawnLocation, spawnRotation, spawnParams);
			if (spawnedItem != nullptr)
			{
				spawnedItem->Initialize(itemToSpawn->ItemData);
			}
		}
	}
}

float AEnemyCharacter::CalculateXP() const
{
	return Level * FMath::RandRange(100, 150);
}

void AEnemyCharacter::UpdateEnemyHUDWorldFacing()
{
	//tristan's code
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}
	
	if (EnemyHUDWorld || EnemyHUDWorld->GetWidget())
	{
		APlayerController* LocalController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

		if (LocalController || LocalController->PlayerCameraManager)
		{
			FVector CameraLocation = LocalController->PlayerCameraManager->GetCameraLocation();
			FVector WidgetLocation = EnemyHUDWorld->GetComponentLocation();

			FVector ToCamera = CameraLocation - WidgetLocation;
			ToCamera.Z = 0.f;

			if (!ToCamera.IsNearlyZero())
			{
				FRotator LookAtRotation = ToCamera.Rotation();
				EnemyHUDWorld->SetWorldRotation(LookAtRotation);
			}

			EnemyHUDWorld->SetVisibility(true);
		}
	}
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateEnemyHUDWorldFacing();
}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

