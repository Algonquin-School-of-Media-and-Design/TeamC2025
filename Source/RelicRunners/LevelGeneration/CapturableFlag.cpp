// Fill out your copyright notice in the Description page of Project Settings.


#include "CapturableFlag.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "RelicRunners/RelicRunnersGameMode.h"
#include "RelicRunners/Game/RelicRunnersGameInstance.h"
#include <Net/UnrealNetwork.h>

// Sets default values
ACapturableFlag::ACapturableFlag():
	Origin(nullptr),
	TriggerBox(nullptr)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Origin = CreateDefaultSubobject<USceneComponent>("Origin");
	RootComponent = Origin;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>("TriggerCollision");
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionProfileName("BlockAll");
	TriggerBox->SetGenerateOverlapEvents(true);
	TriggerBox->SetupAttachment(Origin);

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ACapturableFlag::BeginPlay()
{
	Super::BeginPlay();

	ARelicRunnersGameMode* gamemode = Cast<ARelicRunnersGameMode>(GetWorld()->GetAuthGameMode());

	if (gamemode)
	{
		gamemode->Multicast_IncrementObjective();
	}
	
}

// Called every frame
void ACapturableFlag::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACapturableFlag::Interact_Implementation(ARelicRunnersCharacter* Char)
{
	if (!isActive)
	{
		Multicast_Interacted();
	}
}

void ACapturableFlag::Multicast_Interacted_Implementation()
{
	ARelicRunnersGameMode* gamemode = Cast<ARelicRunnersGameMode>(GetWorld()->GetAuthGameMode());

	URelicRunnersGameInstance* gameInstance = Cast<URelicRunnersGameInstance>(GetGameInstance());

	if (gamemode)
	{
		gamemode->Multicast_DecrementObjective();
	}
	isActive = true;
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("Interacted with Flag")));
}

void ACapturableFlag::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACapturableFlag, isActive);
}