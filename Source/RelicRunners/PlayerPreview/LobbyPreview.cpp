// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPreview.h"
#include "Components/SkeletalMeshComponent.h"

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

    MeshComp->SetIsReplicated(false); // Let animation run locally

}

// Called when the game starts or when spawned
void ALobbyPreview::BeginPlay()
{
	Super::BeginPlay();


	
}

// Called every frame
void ALobbyPreview::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALobbyPreview::SetupFromPlayerState(APlayerState* PS)
{
	// Example: set mesh, nameplate, etc.
	// Cast to your custom PlayerState to get cosmetics
}

