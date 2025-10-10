// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelChangeTrigger.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ALevelChangeTrigger::ALevelChangeTrigger():
	Origin(nullptr),
	TriggerBox(nullptr)
{
	PrimaryActorTick.bCanEverTick = false;

	Origin = CreateDefaultSubobject<USceneComponent>("Origin");
	RootComponent = Origin;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>("TriggerCollision");
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionProfileName("OverlapAll");
	TriggerBox->SetGenerateOverlapEvents(true);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ALevelChangeTrigger::OnTriggerOverlap);
	TriggerBox->SetupAttachment(Origin);
	TriggerBox->SetBoxExtent(FVector(50.0f));
	TriggerBox->SetRelativeLocation(FVector(0.0f, 0.0f, TriggerBox->GetUnscaledBoxExtent().Z));

	LevelTargetTextRender = CreateDefaultSubobject<UTextRenderComponent>("LevelTextRender");
	LevelTargetTextRender->SetupAttachment(Origin);

	SetReplicates(true);
	bAlwaysRelevant = true;
}

void ALevelChangeTrigger::OnConstruction(const FTransform& transform)
{
	TriggerBox->SetRelativeLocation(FVector(0.0f, 0.0f, TriggerBox->GetUnscaledBoxExtent().Z));

	if (TargetLevel.IsNull())
	{
		FString LevelString = FString("No Level");
		LevelTargetTextRender->SetText(FText::FromString(LevelString));
	}
	else
	{
		const FString LevelString = FString(*FPackageName::ObjectPathToPackageName(TargetLevel.ToString()));
		LevelTargetTextRender->SetText(FText::FromString(LevelString));
	}
}

// Called when the game starts or when spawned
void ALevelChangeTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALevelChangeTrigger::OnTriggerOverlap(UPrimitiveComponent* OverlapComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
	{
		MC_ChangeLevel();
	}
}

void ALevelChangeTrigger::ChangeLevel()
{
	UWorld* world = GetWorld();

	if (world == nullptr)
		return;

	if (TargetLevel.IsNull())
		return;

	const FString LevelString = FString(*FPackageName::ObjectPathToPackageName(TargetLevel.ToString() + "?listen"));

	world->ServerTravel(LevelString);
}

void ALevelChangeTrigger::MC_ChangeLevel_Implementation()
{
	ChangeLevel();
}

void ALevelChangeTrigger::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALevelChangeTrigger, TargetLevel);
}
