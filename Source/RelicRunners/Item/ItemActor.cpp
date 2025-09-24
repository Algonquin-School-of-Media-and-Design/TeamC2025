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

#include "ItemActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/LocalFogVolumeComponent.h"
#include "Components/WidgetComponent.h"
#include "ItemStats.h"
#include "ItemData.h"
#include "Kismet/GameplayStatics.h"
#include "RelicRunners/RelicRunnersCharacter.h"
#include "RelicRunners/Inventory/InventoryComponent.h"
#include "RelicRunners/Inventory/InventoryToolTip.h"
#include "Net/UnrealNetwork.h"

AItemActor::AItemActor()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
    SetReplicateMovement(true);

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    Fog = CreateDefaultSubobject<ULocalFogVolumeComponent>(TEXT("Fog"));
    Fog->SetupAttachment(RootComponent);

    TooltipWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("TooltipWidget"));
    TooltipWidgetComponent->SetupAttachment(RootComponent);
    TooltipWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
    TooltipWidgetComponent->SetDrawAtDesiredSize(true);
    TooltipWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 75.f));
    TooltipWidgetComponent->SetRelativeScale3D(FVector(0.1f));
    TooltipWidgetComponent->CastShadow = false;

    static ConstructorHelpers::FClassFinder<UUserWidget> TooltipWidgetBPClass(TEXT("/Game/Inventory/BP_InventoryToolTipWorld"));
    if (TooltipWidgetBPClass.Succeeded())
    {
        TooltipWidgetClass = TooltipWidgetBPClass.Class;
        TooltipWidgetComponent->SetWidgetClass(TooltipWidgetClass);
    }
}

void AItemActor::InitializeTooltipWidget()
{
    if (!TooltipWidgetComponent->GetUserWidgetObject())
    {
        TooltipWidgetComponent->SetWidgetClass(TooltipWidgetClass);
        TooltipWidgetComponent->InitWidget(); // ensures widget is created
    }

    TooltipWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
    TooltipWidgetComponent->SetDrawAtDesiredSize(true);
    TooltipWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 75.f));
    TooltipWidgetComponent->SetRelativeScale3D(FVector(0.1f));
    TooltipWidgetComponent->SetPivot(FVector2D(0.5f, 0.f));
    TooltipWidgetComponent->CastShadow = false;
}


void AItemActor::BeginPlay()
{
    Super::BeginPlay();
}

void AItemActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FRotator Rotation = GetActorRotation();
    Rotation.Yaw += DeltaTime * 45.f;
    SetActorRotation(Rotation);

    if (TooltipWidgetComponent && GetWorld())
    {
        APlayerCameraManager* CamManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
        if (CamManager)
        {
            FVector CameraLocation = CamManager->GetCameraLocation();
            FVector WidgetLocation = TooltipWidgetComponent->GetComponentLocation();

            FRotator LookAtRotation = (CameraLocation - WidgetLocation).Rotation();
            LookAtRotation.Pitch = 0.f;
            LookAtRotation.Roll = 0.f;
            TooltipWidgetComponent->SetWorldRotation(LookAtRotation);
        }
    }
}

void AItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AItemActor, ItemData);
}

void AItemActor::Initialize(const FItemData& InItemData)
{
    ItemData = InItemData;

    if (HasAuthority())
    {
        SetupVisuals();
    }
}

void AItemActor::OnRep_ItemData()
{
    SetupVisuals();
}

void AItemActor::SetupVisuals()
{
    if (!ItemData.ItemStaticMesh) return;

    if (StaticMeshComponent)
    {
        StaticMeshComponent->DestroyComponent();
        StaticMeshComponent = nullptr;
    }

    StaticMeshComponent = NewObject<UStaticMeshComponent>(this, TEXT("DroppedStaticMesh"));
    StaticMeshComponent->SetStaticMesh(ItemData.ItemStaticMesh);
    StaticMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
    StaticMeshComponent->RegisterComponent();
    StaticMeshComponent->SetRelativeLocation(FVector(0.f, 0.f, -45.f));
    StaticMeshComponent->SetSimulatePhysics(false);
    StaticMeshComponent->SetEnableGravity(false);
    StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    StaticMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    StaticMeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    StaticMeshComponent->SetMobility(EComponentMobility::Stationary);
    StaticMeshComponent->CastShadow = false;

    static const TMap<FString, FVector> ItemScaleMap = {
        { "Gloves", FVector(1.25f) }, { "Wrist", FVector(1.0f) },
        { "Chestplate", FVector(1.5f) }, { "Leggings", FVector(1.5f) },
        { "Waist", FVector(1.75f) }, { "Shoulders", FVector(1.75f) },
        { "Backpack", FVector(1.6f) }, { "Shield", FVector(0.4f) },
        { "Sword", FVector(2.0f) }, { "Boots", FVector(1.25f) },
        { "Helmet", FVector(1.6f) }, { "Necklace", FVector(4.0f) },
    };

    if (const FVector* Scale = ItemScaleMap.Find(ItemData.ItemType))
    {
        StaticMeshComponent->SetRelativeScale3D(*Scale);
    }

    // Setup fog
    const auto& RarityData = ItemStats::GetRarityDataMap()[ItemData.Rarity];

    // small tight volume
    Fog->SetRelativeLocation(FVector(0.f, 0.f, -45.f));
    Fog->SetRelativeScale3D(FVector(0.095f, 0.095f, 0.095f));
    Fog->FogPhaseG = 0.0f;
    Fog->HeightFogExtinction = 0.3f;
    Fog->HeightFogFalloff = 1;       // keep >= 1.0 (engine recommendation)
    Fog->FogAlbedo = RarityData.Color;             // tints the scattered light
    Fog->FogEmissive = RarityData.Color * 7.0f;    // strong inner glow
    Fog->MarkRenderStateDirty();

    // IMPORTANT: ensure local fogs show up at close range — set the console var to 0 (or small)
    if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.LocalFogVolume.GlobalStartDistance")))
    {
        CVar->Set(0); // default is 2000 (20m). Set to 0 to disable the "start distance" culling.
    }


    // Tooltip setup
    InitializeTooltipWidget();

    if (UUserWidget* Widget = TooltipWidgetComponent->GetUserWidgetObject())
    {
        if (UInventoryToolTip* Tooltip = Cast<UInventoryToolTip>(Widget))
        {
            Tooltip->Setup(ItemData, RarityData.Color);
        }
    }
}

void AItemActor::ShowTooltip_Implementation(bool bShow)
{
    if (TooltipWidgetComponent)
    {
        TooltipWidgetComponent->SetVisibility(bShow);
    }
}

void AItemActor::Interact_Implementation(ARelicRunnersCharacter* Char)
{
    if (HasAuthority())
    {
        HandlePickup(Char);
    }
    else
    {
        Server_HandlePickup(Char);
    }
}

FItemData AItemActor::GetItemData_Implementation()
{
    return ItemData;
}

void AItemActor::Server_HandlePickup_Implementation(ARelicRunnersCharacter* Char)
{
    if (HasAuthority() && Char)
    {
        HandlePickup(Char);
    }
}

void AItemActor::HandlePickup(ARelicRunnersCharacter* Char)
{
    if (!Char) return;

    if (UInventoryComponent* InventoryComponent = Char->GetInventoryComponent())
    {
        InventoryComponent->AddItem(ItemStats::CreateItemFromData(ItemData, InventoryComponent));
        Destroy();
    }
}
