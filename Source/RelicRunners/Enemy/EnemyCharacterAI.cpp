/************************************************************************************
 * Author:   Tristan Anglin
 * Year:     2025
 *
 * Notice:
 *   This source code is the intellectual property of Tristan Anglin.
 *   It is provided solely for use within:
 *     - The Relic Runners personal project
 *     - The Almost A Dozen project
 *
 *   Any use, distribution, or modification outside of these projects
 *   is strictly prohibited without explicit written permission.
 *
 *   © 2025 Tristan Anglin. All rights reserved.
 ************************************************************************************/

#include "EnemyCharacterAI.h"
#include "EnemyControllerAI.h"
#include "Components/WidgetComponent.h"
#include <Kismet/GameplayStatics.h>
#include "Components/CapsuleComponent.h"
#include <Net/UnrealNetwork.h>
#include "EnemyHUDWorld.h"
#include "RelicRunners/Item/ItemStats.h"
#include "RelicRunners/RelicRunnersCharacter.h"
#include "RelicRunners/Item/ItemMeshData.h"
#include "RelicRunners/Inventory/Inventory.h"
#include "RelicRunners/Inventory/InventoryComponent.h"
#include "RelicRunners/Item/ItemActor.h"
#include "RelicRunners/Spawning System/Director.h"

// Sets default values
AEnemyCharacterAI::AEnemyCharacterAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Mesh attachments
	ChestplateMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ChestplateMesh"));
	ChestplateMesh->SetupAttachment(GetMesh());
	ChestplateMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ChestplateMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	ChestplateMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	ChestplateMesh->SetLeaderPoseComponent(GetMesh());

	BootsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BootsMesh"));
	BootsMesh->SetupAttachment(GetMesh());
	BootsMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BootsMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	BootsMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	BootsMesh->SetLeaderPoseComponent(GetMesh());

	LeggingsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeggingsMesh"));
	LeggingsMesh->SetupAttachment(GetMesh());
	LeggingsMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeggingsMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	LeggingsMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	LeggingsMesh->SetLeaderPoseComponent(GetMesh());

	ShouldersMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShouldersMesh"));
	ShouldersMesh->SetupAttachment(GetMesh());
	ShouldersMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ShouldersMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	ShouldersMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	ShouldersMesh->SetLeaderPoseComponent(GetMesh());

	HelmetMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HelmetMesh"));
	HelmetMesh->SetupAttachment(GetMesh());
	HelmetMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HelmetMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	HelmetMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	HelmetMesh->SetLeaderPoseComponent(GetMesh());

	WaistMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WaistMesh"));
	WaistMesh->SetupAttachment(GetMesh());
	WaistMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WaistMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	WaistMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	WaistMesh->SetLeaderPoseComponent(GetMesh());

	BackpackMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BackpackMesh"));
	BackpackMesh->SetupAttachment(GetMesh());
	BackpackMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BackpackMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	BackpackMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	BackpackMesh->SetLeaderPoseComponent(GetMesh());

	WristMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WristMesh"));
	WristMesh->SetupAttachment(GetMesh());
	WristMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WristMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	WristMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	WristMesh->SetLeaderPoseComponent(GetMesh());

	GlovesMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GlovesMesh"));
	GlovesMesh->SetupAttachment(GetMesh());
	GlovesMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GlovesMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	GlovesMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	GlovesMesh->SetLeaderPoseComponent(GetMesh());

	NecklaceMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("NecklaceMesh"));
	NecklaceMesh->SetupAttachment(GetMesh());
	NecklaceMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	NecklaceMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	NecklaceMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	NecklaceMesh->SetLeaderPoseComponent(GetMesh());

	MainhandItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MainhandMesh"));
	MainhandItemMesh->SetupAttachment(GetMesh(), TEXT("MainhandSocket"));
	MainhandItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MainhandItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	MainhandItemMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

	OffhandItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OffhandMesh"));
	OffhandItemMesh->SetupAttachment(GetMesh(), TEXT("OffhandSocket"));
	OffhandItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OffhandItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	OffhandItemMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);


	AIControllerClass = AEnemyControllerAI::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBP(TEXT("/Game/Characters/Mannequins/Animations/ABP_Manny"));
	if (AnimBP.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(AnimBP.Class);
	}

	//UI
	EnemyHUDWorld = CreateDefaultSubobject<UWidgetComponent>(TEXT("TooltipWidgetComponent"));
	EnemyHUDWorld->SetupAttachment(GetMesh());
	EnemyHUDWorld->SetWorldScale3D(FVector(0.1, 0.1, 0.1));
	EnemyHUDWorld->SetRelativeLocation(FVector(0.f, 0.f, 200.f));
	EnemyHUDWorld->SetWidgetSpace(EWidgetSpace::World);
	EnemyHUDWorld->SetDrawAtDesiredSize(true);
	EnemyHUDWorld->SetVisibility(true);
	EnemyHUDWorld->CastShadow = false;

	//Inventory
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	InventoryComponent->SetIsReplicated(true);

	bReplicates = true;
	SetReplicateMovement(true);
}

void AEnemyCharacterAI::OnRep_CurrentHealth()
{
	if (EnemyHealth <= 0)
	{
		Die(nullptr);
	}
}

void AEnemyCharacterAI::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//stats
	DOREPLIFETIME(AEnemyCharacterAI, EnemyName);
	DOREPLIFETIME(AEnemyCharacterAI, EnemyHealth);
	DOREPLIFETIME(AEnemyCharacterAI, EnemyMaxHealth);
	DOREPLIFETIME(AEnemyCharacterAI, EnemyResource);
	DOREPLIFETIME(AEnemyCharacterAI, EnemyMaxResource);
	DOREPLIFETIME(AEnemyCharacterAI, EnemyLevel);

	//equipped items
	DOREPLIFETIME(AEnemyCharacterAI, ReplicatedChestplateMesh);
	DOREPLIFETIME(AEnemyCharacterAI, ReplicatedBootsMesh);
	DOREPLIFETIME(AEnemyCharacterAI, ReplicatedLeggingsMesh);
	DOREPLIFETIME(AEnemyCharacterAI, ReplicatedShouldersMesh);
	DOREPLIFETIME(AEnemyCharacterAI, ReplicatedHelmetMesh);
	DOREPLIFETIME(AEnemyCharacterAI, ReplicatedWaistMesh);
	DOREPLIFETIME(AEnemyCharacterAI, ReplicatedBackpackMesh);
	DOREPLIFETIME(AEnemyCharacterAI, ReplicatedGlovesMesh);
	DOREPLIFETIME(AEnemyCharacterAI, ReplicatedWristMesh);
	DOREPLIFETIME(AEnemyCharacterAI, ReplicatedNecklaceMesh);
	DOREPLIFETIME(AEnemyCharacterAI, ReplicatedMainhandMesh);
	DOREPLIFETIME(AEnemyCharacterAI, ReplicatedOffhandMesh);
}

float AEnemyCharacterAI::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageApplied = FMath::Min(EnemyHealth, (int32)DamageAmount);
	EnemyHealth -= DamageApplied;

	UE_LOG(LogTemp, Warning, TEXT("%s took %f damage. Remaining health: %d"), *GetName(), DamageAmount, EnemyHealth);

	if (EnemyHealth <= 0)
	{
		Die(EventInstigator);
	}

	return DamageApplied;
}

void AEnemyCharacterAI::Die(AController* KillerController)
{
	
	// Get all currently equipped items
	if (!Alive) return;
	if (!HasAuthority()) return;

	Alive = false;

	TArray<FEquippedItemEntry> EquippedItems = InventoryComponent->GetEquippedItems();
	if (EquippedItems.Num() > 0)
	{
		// Pick a random equipped item
		int32 RandomIndex = FMath::RandRange(0, EquippedItems.Num() - 1);
		UItemObject* DroppedItem = EquippedItems[RandomIndex].Item;

		// Spawn new dropped item
		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.Instigator = this;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		// Desired drop location
		FVector SpawnLoc = GetActorLocation() + GetActorForwardVector() * 100.f;
		AItemActor* Dropped = GetWorld()->SpawnActor<AItemActor>(
			ItemActorClass, SpawnLoc, FRotator::ZeroRotator, Params);

		if (Dropped && DroppedItem)
		{
			Dropped->Initialize(DroppedItem->ItemData);
		}
	}

	// Grant XP to killer
	if (KillerController)
	{
		if (ARelicRunnersCharacter* KillerCharacter = Cast<ARelicRunnersCharacter>(KillerController->GetPawn()))
		{
			int XPGranted = CalculateXPReward();
			KillerCharacter->AddExperience(XPGranted);

			UE_LOG(LogTemp, Log, TEXT("%s gained %d XP for killing %s"), *KillerCharacter->GetName(), XPGranted, *GetName());
		}
	}

	// Stop AI movement
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();
	}

	// Disable capsule collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Destroy the enemy HUD widget
	if (EnemyHUDWorld)
	{
		EnemyHUDWorld->DestroyComponent();
	}

	// Enable ragdoll
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

	// Destroy after 10 seconds
	SetLifeSpan(3.f);
}

int AEnemyCharacterAI::CalculateXPReward() const
{
	return EnemyLevel * FMath::RandRange(3,7);
}

// Called when the game starts or when spawned
void AEnemyCharacterAI::BeginPlay()
{
	Super::BeginPlay();

	if (ItemMeshDataClass)
	{
		ItemMeshData = ItemMeshDataClass->GetDefaultObject<UItemMeshData>();
	}

	if (EnemyHUDWorld)
	{
		UUserWidget* Widget = EnemyHUDWorld->GetUserWidgetObject();
		if (Widget)
		{
			UEnemyHUDWorld* EnemyHUD = Cast<UEnemyHUDWorld>(Widget);
			if (EnemyHUD)
			{
				EnemyHUD->InitWidgetWithCharacter(this);
			}
		}
	}

	if (HasAuthority())
	{
		UWorld* World = GetWorld();
		APawn* enemy = static_cast<APawn*>(this);

		GetWorld()->GetTimerManager().SetTimerForNextTick([World, enemy] {
			ADirector* Director = static_cast<ADirector*>(UGameplayStatics::GetActorOfClass(World, ADirector::StaticClass()));
			Director->AddEnemy(enemy);
			enemy->OnDestroyed.AddDynamic(Director, &ADirector::RemoveEnemy);
			});
	}

	TryBindInventoryDelegates();

	InitializeStartingEquipment();
}

void AEnemyCharacterAI::TryBindInventoryDelegates()
{
	if (Inventory && InventoryComponent)
	{
		Inventory->SetInventoryComponent(InventoryComponent);
		InventoryComponent->UpdateTotalEquippedStats(this);

		Inventory->RefreshInventoryUI();
		Inventory->RefreshEquippedUI();

		UE_LOG(LogTemp, Warning, TEXT("[UI] Bound inventory component delegates successfully."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[UI] Waiting for Inventory or InventoryComponent..."));
	}
}

void AEnemyCharacterAI::InitializeStartingEquipment()
{
	if (!InventoryComponent) return;

	TArray<FString> StartingItems = {
		"Sword", "Shield", "Helmet", "Shoulders", "Chestplate",
		"Leggings", "Boots", "Backpack", "Necklace",
		"Waist", "Wrist", "Gloves"
	};

	for (const FString& ItemName : StartingItems)
	{
		UItemObject* Item = ItemStats::CreateItemFromData(ItemStats::CreateSpecificItemData(EnemyLevel, ItemName, ItemMeshData), InventoryComponent);

		if (Item)
		{
			InventoryComponent->AddItem(Item);
			InventoryComponent->EquipItem(Item);
			UpdateItemVisuals(Item);
		}
	}

	InventoryComponent->UpdateTotalEquippedStats(this);
	EnemyHealth = EnemyMaxHealth;
}

void AEnemyCharacterAI::UpdateItemVisuals(UObject* MeshAsset, const FString& ItemType)
{
	if (ItemType == "Sword" || ItemType == "Shield")
	{
		if (UStaticMeshComponent* TargetMesh = GetStaticMeshComponentByItemType(ItemType))
		{
			TargetMesh->SetStaticMesh(Cast<UStaticMesh>(MeshAsset));
			OnRep_MeshUpdate(MeshAsset, ItemType);
		}
	}
	else
	{
		if (USkeletalMeshComponent* TargetMesh = GetSkeletalMeshComponentByItemType(ItemType))
		{
			TargetMesh->SetSkeletalMesh(Cast<USkeletalMesh>(MeshAsset));
			OnRep_MeshUpdate(MeshAsset, ItemType);
		}
	}
}

void AEnemyCharacterAI::UpdateItemVisuals(UItemObject* Item)
{
	if (!Item) return;

	const FString& ItemType = Item->ItemData.ItemType;
	const FString MeshPath = GetMeshPathByItemType(ItemType);

	if (ItemType == "Sword" || ItemType == "Shield")
	{
		if (UStaticMesh* LoadedMesh = LoadObject<UStaticMesh>(nullptr, *MeshPath))
		{
			if (UStaticMeshComponent* TargetMesh = GetStaticMeshComponentByItemType(ItemType))
			{
				TargetMesh->SetStaticMesh(LoadedMesh);
				OnRep_MeshUpdate(LoadedMesh, ItemType);
			}
		}
	}
	else
	{
		if (USkeletalMesh* LoadedMesh = LoadObject<USkeletalMesh>(nullptr, *MeshPath))
		{
			if (USkeletalMeshComponent* TargetMesh = GetSkeletalMeshComponentByItemType(ItemType))
			{
				TargetMesh->SetSkeletalMesh(LoadedMesh);
				OnRep_MeshUpdate(LoadedMesh, ItemType);
			}
		}
	}
}

void AEnemyCharacterAI::Server_EquipItemByID_Implementation(FGuid ID)
{
	if (!InventoryComponent) return;

	UItemObject** ItemPtr = InventoryComponent->InventoryItems.FindByPredicate([&](UItemObject* Obj)
		{
			return Obj && Obj->ItemData.UniqueID == ID;
		});

	if (ItemPtr && *ItemPtr)
	{
		UItemObject* Item = *ItemPtr;
		InventoryComponent->EquipItem(Item);
		UpdateItemVisuals(Item);
		InventoryComponent->UpdateTotalEquippedStats(this);
	}
}

void AEnemyCharacterAI::Client_UpdateEquippedStats_Implementation(const FEquippedStatsSummary& NewStats)
{
	if (InventoryComponent)
	{
		InventoryComponent->CachedEquippedStats = NewStats;
		InventoryComponent->OnStatsChanged.Broadcast(NewStats);
	}
}

void AEnemyCharacterAI::Server_SetMaxHealth_Implementation(int health)
{
	EnemyMaxHealth = health;
}

void AEnemyCharacterAI::Server_UnequipItemByID_Implementation(FGuid ID)
{
	if (!InventoryComponent) return;

	UItemObject* EquippedItem = nullptr;
	FString ItemType;

	for (FEquippedItemEntry& Entry : InventoryComponent->EquippedItems)
	{
		if (Entry.Item && Entry.Item->ItemData.UniqueID == ID)
		{
			EquippedItem = Entry.Item;
			ItemType = Entry.ItemType;
			break;
		}
	}

	if (EquippedItem)
	{
		InventoryComponent->UnequipItem(EquippedItem);
		UpdateItemVisuals(nullptr, ItemType);
		InventoryComponent->UpdateTotalEquippedStats(this);
	}
}

void AEnemyCharacterAI::OnRep_MeshUpdate(UObject* MeshAsset, const FString& ItemType)
{
	SetReplicatedMeshByItemType(MeshAsset, ItemType);
}

FString AEnemyCharacterAI::GetMeshPathByItemType(const FString& ItemType)
{
	return FString::Printf(TEXT("/Game/ThirdPerson/Meshes/Items/%s/%s.%s"),
		*ItemType, *ItemType, *ItemType);
}

USkeletalMeshComponent* AEnemyCharacterAI::GetSkeletalMeshComponentByItemType(const FString& ItemType)
{
	if (ItemType == "Chestplate") return ChestplateMesh;
	if (ItemType == "Leggings") return LeggingsMesh;
	if (ItemType == "Boots") return BootsMesh;
	if (ItemType == "Shoulders") return ShouldersMesh;
	if (ItemType == "Helmet") return HelmetMesh;
	if (ItemType == "Waist") return WaistMesh;
	if (ItemType == "Backpack") return BackpackMesh;
	if (ItemType == "Gloves") return GlovesMesh;
	if (ItemType == "Wrist") return WristMesh;
	if (ItemType == "Necklace") return NecklaceMesh;
	return nullptr;
}

UStaticMeshComponent* AEnemyCharacterAI::GetStaticMeshComponentByItemType(const FString& ItemType)
{
	if (ItemType == "Sword") return MainhandItemMesh;
	if (ItemType == "Shield") return OffhandItemMesh;
	return nullptr;
}

void AEnemyCharacterAI::Server_SetReplicatedMeshByItemType_Implementation(UObject* MeshAsset, const FString& ItemType)
{
	SetReplicatedMeshByItemType(MeshAsset, ItemType);
}

void AEnemyCharacterAI::OnRep_ChestplateMesh()
{
	if (ChestplateMesh) ChestplateMesh->SetSkeletalMesh(ReplicatedChestplateMesh);
}

void AEnemyCharacterAI::OnRep_BootsMesh()
{
	if (BootsMesh) BootsMesh->SetSkeletalMesh(ReplicatedBootsMesh);
}

void AEnemyCharacterAI::OnRep_LeggingsMesh()
{
	if (LeggingsMesh) LeggingsMesh->SetSkeletalMesh(ReplicatedLeggingsMesh);
}

void AEnemyCharacterAI::OnRep_ShouldersMesh()
{
	if (ShouldersMesh) ShouldersMesh->SetSkeletalMesh(ReplicatedShouldersMesh);
}

void AEnemyCharacterAI::OnRep_HelmetMesh()
{
	if (HelmetMesh) HelmetMesh->SetSkeletalMesh(ReplicatedHelmetMesh);
}

void AEnemyCharacterAI::OnRep_WaistMesh()
{
	if (WaistMesh) WaistMesh->SetSkeletalMesh(ReplicatedWaistMesh);
}

void AEnemyCharacterAI::OnRep_BackpackMesh()
{
	if (BackpackMesh) BackpackMesh->SetSkeletalMesh(ReplicatedBackpackMesh);
}

void AEnemyCharacterAI::OnRep_GlovesMesh()
{
	if (GlovesMesh) GlovesMesh->SetSkeletalMesh(ReplicatedGlovesMesh);
}

void AEnemyCharacterAI::OnRep_WristMesh()
{
	if (WristMesh) WristMesh->SetSkeletalMesh(ReplicatedWristMesh);
}

void AEnemyCharacterAI::OnRep_NecklaceMesh()
{
	if (NecklaceMesh) NecklaceMesh->SetSkeletalMesh(ReplicatedNecklaceMesh);
}

void AEnemyCharacterAI::OnRep_MainhandMesh()
{
	if (MainhandItemMesh) MainhandItemMesh->SetStaticMesh(ReplicatedMainhandMesh);
}

void AEnemyCharacterAI::OnRep_OffhandMesh()
{
	if (OffhandItemMesh) OffhandItemMesh->SetStaticMesh(ReplicatedOffhandMesh);
}

void AEnemyCharacterAI::SetReplicatedMeshByItemType(UObject* MeshAsset, const FString& ItemType)
{

	if (!HasAuthority())
	{
		Server_SetReplicatedMeshByItemType(MeshAsset, ItemType);
		return;
	}

	if (ItemType == "Chestplate")
	{
		ReplicatedChestplateMesh = Cast<USkeletalMesh>(MeshAsset);
		OnRep_ChestplateMesh();
	}
	else if (ItemType == "Boots")
	{
		ReplicatedBootsMesh = Cast<USkeletalMesh>(MeshAsset);
		OnRep_BootsMesh();
	}
	else if (ItemType == "Leggings")
	{
		ReplicatedLeggingsMesh = Cast<USkeletalMesh>(MeshAsset);
		OnRep_LeggingsMesh();
	}
	else if (ItemType == "Shoulders")
	{
		ReplicatedShouldersMesh = Cast<USkeletalMesh>(MeshAsset);
		OnRep_ShouldersMesh();
	}
	else if (ItemType == "Helmet")
	{
		ReplicatedHelmetMesh = Cast<USkeletalMesh>(MeshAsset);
		OnRep_HelmetMesh();
	}
	else if (ItemType == "Waist")
	{
		ReplicatedWaistMesh = Cast<USkeletalMesh>(MeshAsset);
		OnRep_WaistMesh();
	}
	else if (ItemType == "Backpack")
	{
		ReplicatedBackpackMesh = Cast<USkeletalMesh>(MeshAsset);
		OnRep_BackpackMesh();
	}
	else if (ItemType == "Gloves")
	{
		ReplicatedGlovesMesh = Cast<USkeletalMesh>(MeshAsset);
		OnRep_GlovesMesh();
	}
	else if (ItemType == "Wrist")
	{
		ReplicatedWristMesh = Cast<USkeletalMesh>(MeshAsset);
		OnRep_WristMesh();
	}
	else if (ItemType == "Necklace")
	{
		ReplicatedNecklaceMesh = Cast<USkeletalMesh>(MeshAsset);
		OnRep_NecklaceMesh();
	}
	else if (ItemType == "Sword" || ItemType == "Mainhand")
	{
		ReplicatedMainhandMesh = Cast<UStaticMesh>(MeshAsset);
		OnRep_MainhandMesh();
	}
	else if (ItemType == "Shield" || ItemType == "Offhand")
	{
		ReplicatedOffhandMesh = Cast<UStaticMesh>(MeshAsset);
		OnRep_OffhandMesh();
	}
}

// Called every frame
void AEnemyCharacterAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateEnemyHUDWorldFacing();
}

// Called to bind functionality to input
void AEnemyCharacterAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyCharacterAI::UpdateEnemyHUDWorldFacing()
{
	if (!EnemyHUDWorld || !EnemyHUDWorld->GetWidget()) return;

	// Skip on server
	if (GetNetMode() == NM_DedicatedServer)
		return;

	APlayerController* LocalController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!LocalController || !LocalController->PlayerCameraManager) return;

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


