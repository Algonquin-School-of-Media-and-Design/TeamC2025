// Fill out your copyright notice in the Description page of Project Settings.


//#include "WarBanner.h"
//#include "WarBannerAbility.h"
//#include "Net/UnrealNetwork.h"

//AWarBanner::AWarBanner()
//{
//	PrimaryActorTick.bCanEverTick = true;
//
//	BannerTimelineComp = CreateDefaultSubobject<UTimelineComponent>(TEXT("BannerTimelineComp"));
//
//	BannerMesh = CreateDefaultSubobject<UStaticMeshComponent>("Banner");
//	BannerMesh->SetupAttachment(RootComponent);
//
//	SetReplicates(true);
//
//}
//
//void AWarBanner::BeginPlay()
//{
//	Super::BeginPlay();
//
//	if (BannerTimelineFloatCurve)
//	{
//		FOnTimelineFloat UpdateFunctionFloat;
//		UpdateFunctionFloat.BindUFunction(this, FName("UpdateTimelineComp"));
//		BannerTimelineComp->AddInterpFloat(BannerTimelineFloatCurve, UpdateFunctionFloat);
//
//		FOnTimelineEvent TimelineFinishedEvent;
//		TimelineFinishedEvent.BindUFunction(this, FName("Server_Despawn"));
//		BannerTimelineComp->SetTimelineFinishedFunc(TimelineFinishedEvent);
//
//		BannerTimelineComp->SetLooping(false);
//	}
//
//	BannerMesh->SetVisibility(false);
//	SetActorTickEnabled(false);
//}
//
//void AWarBanner::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//	if (IsAlive)
//	{
//		DrawDebugSphere(GetWorld(), GetActorLocation(), Radius, 32, FColor::White);
//	}
//}
//
//void AWarBanner::Initialize(AActor* newOwner)
//{
//	Owner = newOwner;
//}
//
//void AWarBanner::UpdateTimelineComp(float Output)
//{
//	LifeTime = Output;
//	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("Banner LifeTime: %f"), LifeTime));
//}
//
//void AWarBanner::Server_Spawn_Implementation(float duration, float damageAmount, float areaRadius)
//{
//	SetActorTickEnabled(true);
//	BannerMesh->SetVisibility(true);
//
//	BannerTimelineComp->SetPlayRate(1.0f / duration);
//	BannerTimelineComp->PlayFromStart();
//	IsAlive = true;
//	LifeTime = duration;
//	Damage = damageAmount;
//	Radius = areaRadius;
//}
//
//void AWarBanner::Server_Despawn_Implementation()
//{
//	IsAlive = false;
//	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("Timeline Ended")));
//
//	AWarBannerAbility* ownerAbility = Cast<AWarBannerAbility>(Owner);
//	if (ownerAbility != nullptr)
//	{
//		ownerAbility->EndAbility();
//	}
//
//	SetActorTickEnabled(false);
//	BannerMesh->SetVisibility(false);
//}
//
//void AWarBanner::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	DOREPLIFETIME(AWarBanner, IsAlive);
//	DOREPLIFETIME(AWarBanner, LifeTime);
//	DOREPLIFETIME(AWarBanner, Damage);
//	DOREPLIFETIME(AWarBanner, Radius);
//}


