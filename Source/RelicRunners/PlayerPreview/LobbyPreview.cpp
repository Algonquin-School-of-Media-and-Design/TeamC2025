// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPreview.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include <RelicRunners/PlayerState/RelicRunnersPlayerState.h>
#include <Net/UnrealNetwork.h>
#include <RelicRunners/PlayerHUD/LobbyHUDWorld.h>

// Sets default values
ALobbyPreview::ALobbyPreview()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
    SetReplicateMovement(true);

    MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComp;

    static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBP(
        TEXT("/Game/Characters/Mannequins/Animations/ABP_Quinn.ABP_Quinn_C")
    );

    if (AnimBP.Succeeded())
    {
        MeshComp->SetAnimInstanceClass(AnimBP.Class);
    }

    MeshComp->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
    MeshComp->SetComponentTickEnabled(true);
    MeshComp->bPauseAnims = false;
    MeshComp->bNoSkeletonUpdate = false;
    MeshComp->bEnableUpdateRateOptimizations = false;
    MeshComp->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
    MeshComp->bOnlyAllowAutonomousTickPose = false;

    MeshComp->SetIsReplicated(false);

    LobbyHUDWorld = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
    LobbyHUDWorld->SetupAttachment(MeshComp);
    LobbyHUDWorld->SetWorldScale3D(FVector(0.1, 0.1, 0.1));
    LobbyHUDWorld->SetRelativeLocation(FVector(0.f, 0.f, 200.f));
    LobbyHUDWorld->SetWidgetSpace(EWidgetSpace::World);
    LobbyHUDWorld->SetDrawAtDesiredSize(true);
    LobbyHUDWorld->SetVisibility(true);
    LobbyHUDWorld->CastShadow = false;

}

void ALobbyPreview::BeginPlay()
{
	Super::BeginPlay();

    if (LobbyHUDWorld)
    {
        if (UUserWidget* Widget = LobbyHUDWorld->GetUserWidgetObject())
        {
            if (ULobbyHUDWorld* LobbyWidget = Cast<ULobbyHUDWorld>(Widget))
            {
                LobbyWidget->OwningPreview = this;
            }
        }
    }
	
}

void ALobbyPreview::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALobbyPreview::SetupFromPlayerState(ARelicRunnersPlayerState* PS)
{
    if (!HasAuthority() || !PS) return;

    LinkedPlayerState = PS;
    OnRep_LinkedPlayerState();
}

void ALobbyPreview::OnPlayerClassChanged(FName NewClass)
{
    if (!LinkedPlayerState) return;

    UE_LOG(LogTemp, Log, TEXT("[LobbyPreview] %s -> class %s"),
        *GetNameSafe(LinkedPlayerState), *NewClass.ToString());

}

void ALobbyPreview::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ALobbyPreview, LinkedPlayerState);
}

void ALobbyPreview::OnRep_LinkedPlayerState()
{
    if (LinkedPlayerState)
    {
        LinkedPlayerState->OnSelectedClassChanged.AddUObject(this, &ALobbyPreview::OnPlayerClassChanged);
        OnPlayerClassChanged(LinkedPlayerState->SelectedClass);
    }
}