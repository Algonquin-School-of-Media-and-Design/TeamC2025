// Fill out your copyright notice in the Description page of Project Settings.


#include "PayloadActor.h"
#include "Components/SplineComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "RelicRunners/RelicRunnersGameState.h"
#include "RelicRunners/Game/RelicRunnersGameInstance.h"
#include <Net/UnrealNetwork.h>

// Sets default values
APayloadActor::APayloadActor() :
	DeliverySpline(nullptr),
	Distance(0),
	MaxSpeed(0),
	CurrentSpeed(0),
	IsActive(false),
	HasReachedDestination(false)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Origin = CreateDefaultSubobject<USceneComponent>("Origin");
	RootComponent = Origin;

	PayloadBody = CreateDefaultSubobject<UBoxComponent>("PayloadBody");
	PayloadBody->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PayloadBody->SetCollisionProfileName("BlockAllDynamic");
	PayloadBody->SetGenerateOverlapEvents(true);
	PayloadBody->SetupAttachment(Origin);

	TriggerCollision = CreateDefaultSubobject<USphereComponent>("TriggerCollision");
	TriggerCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerCollision->SetCollisionProfileName("OverlapAll");
	TriggerCollision->SetGenerateOverlapEvents(true);
	TriggerCollision->OnComponentBeginOverlap.AddDynamic(this, &APayloadActor::OnTriggerOverlap);
	TriggerCollision->OnComponentEndOverlap.AddDynamic(this, &APayloadActor::OnOverlapEnd);
	TriggerCollision->SetupAttachment(Origin);

	bReplicates = true;
	bAlwaysRelevant = true;
}

// Called when the game starts or when spawned
void APayloadActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APayloadActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (DeliverySpline)
	{
		Distance = FMath::FInterpConstantTo(Distance, DeliverySpline->GetSplineLength(), DeltaTime, CurrentSpeed);

		SetActorTransform(DeliverySpline->GetTransformAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World));

		if (Distance >= DeliverySpline->GetSplineLength() && !HasReachedDestination)
		{
			HasReachedDestination = true;
			ARelicRunnersGameState* gameState = Cast<ARelicRunnersGameState>(GetWorld()->GetGameState());

			if (gameState)
			{
				gameState->Server_DecrementObjective(EObjectiveType::MoveThePayload);
			}
			GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Yellow, FString::Printf(TEXT("Payload has reached destination")));
		}
	}
}

void APayloadActor::Initialize(USplineComponent* newSpline, float newSpeed)
{
	DeliverySpline = newSpline;
	MaxSpeed = newSpeed;
}

void APayloadActor::OnTriggerOverlap(UPrimitiveComponent* OverlapComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("Player"))
	{
		CurrentSpeed = MaxSpeed;
	}
}

void APayloadActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag("Player"))
	{
		CurrentSpeed = 0.0f;
	}
}

void APayloadActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APayloadActor, Distance);
	DOREPLIFETIME(APayloadActor, MaxSpeed);
	DOREPLIFETIME(APayloadActor, CurrentSpeed);
	DOREPLIFETIME(APayloadActor, IsActive);
	DOREPLIFETIME(APayloadActor, HasReachedDestination);
}