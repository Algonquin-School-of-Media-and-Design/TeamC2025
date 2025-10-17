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

#include "PlayerPreview.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "RelicRunners/Inventory/Inventory.h"
#include "Components/SceneCaptureComponent2D.h"
#include "GameFramework/SpringArmComponent.h"
#include "RelicRunners/RelicRunnersCharacter.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
APlayerPreview::APlayerPreview()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComponent);

	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(RootComponent);

	SceneCaptureComponent2D = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	SceneCaptureComponent2D->SetupAttachment(RootComponent);
	SceneCaptureComponent2D->bCaptureEveryFrame = true;
	SceneCaptureComponent2D->bCaptureOnMovement = false;

	// Mesh attachments
	ChestplateMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ChestplateMesh"));
	ChestplateMesh->SetupAttachment(BodyMesh);
	ChestplateMesh->SetLeaderPoseComponent(BodyMesh);

	BootsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BootsMesh"));
	BootsMesh->SetupAttachment(BodyMesh);
	BootsMesh->SetLeaderPoseComponent(BodyMesh);

	LeggingsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeggingsMesh"));
	LeggingsMesh->SetupAttachment(BodyMesh);
	LeggingsMesh->SetLeaderPoseComponent(BodyMesh);

	ShouldersMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShouldersMesh"));
	ShouldersMesh->SetupAttachment(BodyMesh);
	ShouldersMesh->SetLeaderPoseComponent(BodyMesh);

	HelmetMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HelmetMesh"));
	HelmetMesh->SetupAttachment(BodyMesh);
	HelmetMesh->SetLeaderPoseComponent(BodyMesh);

	WaistMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WaistMesh"));
	WaistMesh->SetupAttachment(BodyMesh);
	WaistMesh->SetLeaderPoseComponent(BodyMesh);

	BackpackMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BackpackMesh"));
	BackpackMesh->SetupAttachment(BodyMesh);
	BackpackMesh->SetLeaderPoseComponent(BodyMesh);

	WristMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WristMesh"));
	WristMesh->SetupAttachment(BodyMesh);
	WristMesh->SetLeaderPoseComponent(BodyMesh);

	GlovesMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GlovesMesh"));
	GlovesMesh->SetupAttachment(BodyMesh);
	GlovesMesh->SetLeaderPoseComponent(BodyMesh);

	NecklaceMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("NecklaceMesh"));
	NecklaceMesh->SetupAttachment(BodyMesh);
	NecklaceMesh->SetLeaderPoseComponent(BodyMesh);

	MainhandItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MainhandMesh"));
	MainhandItemMesh->SetupAttachment(BodyMesh, TEXT("MainhandSocket"));

	OffhandItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OffhandMesh"));
	OffhandItemMesh->SetupAttachment(BodyMesh, TEXT("OffhandSocket"));

	bReplicates = true;
	SetReplicateMovement(true);
	SetActorHiddenInGame(false);
}

void APlayerPreview::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APlayerPreview::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerPreview::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void APlayerPreview::ClearAllPreviewMeshes()
{
	// Skeletal meshes (armor, accessories)
	ChestplateMesh->SetSkeletalMesh(nullptr);
	BootsMesh->SetSkeletalMesh(nullptr);
	LeggingsMesh->SetSkeletalMesh(nullptr);
	ShouldersMesh->SetSkeletalMesh(nullptr);
	HelmetMesh->SetSkeletalMesh(nullptr);
	WaistMesh->SetSkeletalMesh(nullptr);
	BackpackMesh->SetSkeletalMesh(nullptr);
	WristMesh->SetSkeletalMesh(nullptr);
	GlovesMesh->SetSkeletalMesh(nullptr);
	NecklaceMesh->SetSkeletalMesh(nullptr);

	// Static meshes (weapons)
	MainhandItemMesh->SetStaticMesh(nullptr);
	OffhandItemMesh->SetStaticMesh(nullptr);
}

UTextureRenderTarget2D* APlayerPreview::CreateRenderTarget()
{
	// Create and assign a unique render target
	UTextureRenderTarget2D* NewRenderTarget = NewObject<UTextureRenderTarget2D>();
	NewRenderTarget->RenderTargetFormat = RTF_RGBA16f;
	NewRenderTarget->InitAutoFormat(400, 1024); 
	NewRenderTarget->ClearColor = FLinearColor(0, 0, 0, 0); 
	NewRenderTarget->bAutoGenerateMips = false;
	NewRenderTarget->UpdateResourceImmediate(true);

	SceneCaptureComponent2D->TextureTarget = NewRenderTarget;
	SceneCaptureComponent2D->CaptureSource = ESceneCaptureSource::SCS_SceneColorHDR;
	SceneCaptureComponent2D->ShowFlags.SetAtmosphere(false);
	SceneCaptureComponent2D->ShowFlags.SetFog(false);
	SceneCaptureComponent2D->ShowFlags.SetDynamicShadows(false);

	UMaterialInterface* PostMat = LoadObject<UMaterialInterface>(
		nullptr, TEXT("/Game/ThirdPerson/Blueprints/PlayerPreview/M_PlayerPreviewPostProcess.M_PlayerPreviewPostProcess"));

	if (PostMat)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SceneCapture] Successfully set post process material."));
		SceneCaptureComponent2D->PostProcessSettings.AddBlendable(PostMat, 1.0f);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[SceneCapture] Failed to set post process material."));
	}

	RenderTarget = NewRenderTarget;
	return RenderTarget;
}

USkeletalMeshComponent* APlayerPreview::GetSkeletalMeshComponentByItemType(const FString& ItemType)
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

UStaticMeshComponent* APlayerPreview::GetStaticMeshComponentByItemType(const FString& ItemType)
{
	if (ItemType == "Sword") return MainhandItemMesh;
	if (ItemType == "Shield") return OffhandItemMesh;
	return nullptr;
}

void APlayerPreview::SetPreviewMeshByItemType(const FString& ItemType, UObject* MeshAsset)
{
	if (ItemType == "Chestplate")
	{
		USkeletalMesh* Mesh = Cast<USkeletalMesh>(MeshAsset);
		if (ChestplateMesh && Mesh) ChestplateMesh->SetSkeletalMesh(Mesh);
	}
	else if (ItemType == "Boots")
	{
		USkeletalMesh* Mesh = Cast<USkeletalMesh>(MeshAsset);
		if (BootsMesh && Mesh) BootsMesh->SetSkeletalMesh(Mesh);
	}
	else if (ItemType == "Leggings")
	{
		USkeletalMesh* Mesh = Cast<USkeletalMesh>(MeshAsset);
		if (LeggingsMesh && Mesh) LeggingsMesh->SetSkeletalMesh(Mesh);
	}
	else if (ItemType == "Shoulders")
	{
		USkeletalMesh* Mesh = Cast<USkeletalMesh>(MeshAsset);
		if (ShouldersMesh && Mesh) ShouldersMesh->SetSkeletalMesh(Mesh);
	}
	else if (ItemType == "Helmet")
	{
		USkeletalMesh* Mesh = Cast<USkeletalMesh>(MeshAsset);
		if (HelmetMesh && Mesh) HelmetMesh->SetSkeletalMesh(Mesh);
	}
	else if (ItemType == "Waist")
	{
		USkeletalMesh* Mesh = Cast<USkeletalMesh>(MeshAsset);
		if (WaistMesh && Mesh) WaistMesh->SetSkeletalMesh(Mesh);
	}
	else if (ItemType == "Backpack")
	{
		USkeletalMesh* Mesh = Cast<USkeletalMesh>(MeshAsset);
		if (BackpackMesh && Mesh) BackpackMesh->SetSkeletalMesh(Mesh);
	}
	else if (ItemType == "Gloves")
	{
		USkeletalMesh* Mesh = Cast<USkeletalMesh>(MeshAsset);
		if (GlovesMesh && Mesh) GlovesMesh->SetSkeletalMesh(Mesh);
	}
	else if (ItemType == "Wrist")
	{
		USkeletalMesh* Mesh = Cast<USkeletalMesh>(MeshAsset);
		if (WristMesh && Mesh) WristMesh->SetSkeletalMesh(Mesh);
	}
	else if (ItemType == "Necklace")
	{
		USkeletalMesh* Mesh = Cast<USkeletalMesh>(MeshAsset);
		if (NecklaceMesh && Mesh) NecklaceMesh->SetSkeletalMesh(Mesh);
	}
	else if (ItemType == "Sword" || ItemType == "Mainhand")
	{
		UStaticMesh* Mesh = Cast<UStaticMesh>(MeshAsset);
		if (MainhandItemMesh && Mesh) MainhandItemMesh->SetStaticMesh(Mesh);
	}
	else if (ItemType == "Shield" || ItemType == "Offhand")
	{
		UStaticMesh* Mesh = Cast<UStaticMesh>(MeshAsset);
		if (OffhandItemMesh && Mesh) OffhandItemMesh->SetStaticMesh(Mesh);
	}
}