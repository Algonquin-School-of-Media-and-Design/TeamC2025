// Fill out your copyright notice in the Description page of Project Settings.


#include "CapturableFlag.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "RelicRunners/RelicRunnersGameMode.h"

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
}

// Called when the game starts or when spawned
void ACapturableFlag::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACapturableFlag::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACapturableFlag::Interact_Implementation(ARelicRunnersCharacter* Char)
{
	isActive = true;
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("Interacted with Flag")));
}

