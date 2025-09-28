// Fill out your copyright notice in the Description page of Project Settings.


#include "GeneratedFloor.h"
#include "LevelGenerator.h"
#include "Components/SceneComponent.h"
#include "Components/ArrowComponent.h"

// Sets default values
AGeneratedFloor::AGeneratedFloor() :
	GeneratorOwner(nullptr),
	Origin(nullptr),
	MainState(EMainFloorState::Blank),
	HorizontalShape(EHorizontalFloorShape::Blank),
	VerticalShape(EVerticalFloorShape::Blank)

{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Origin = CreateDefaultSubobject<USceneComponent>("Origin");
	RootComponent = Origin;

	DebugUpArrow = CreateDefaultSubobject<UArrowComponent>("UpArrow");
	DebugUpArrow->SetupAttachment(Origin);
	DebugUpArrow->SetArrowColor(FLinearColor(0.0f, 0.0f, 0.0f));

	DebugDownArrow = CreateDefaultSubobject<UArrowComponent>("DownArrow");
	DebugDownArrow->SetupAttachment(Origin);
	DebugDownArrow->SetArrowColor(FLinearColor(0.0f, 0.0f, 0.0f));

	DebugLeftArrow = CreateDefaultSubobject<UArrowComponent>("LeftArrow");
	DebugLeftArrow->SetupAttachment(Origin);
	DebugLeftArrow->SetArrowColor(FLinearColor(0.0f, 0.0f, 0.0f));

	DebugRightArrow = CreateDefaultSubobject<UArrowComponent>("RightArrow");
	DebugRightArrow->SetupAttachment(Origin);
	DebugRightArrow->SetArrowColor(FLinearColor(0.0f, 0.0f, 0.0f));

}

void AGeneratedFloor::BeginPlay()
{
	Super::BeginPlay();
}

void AGeneratedFloor::InitializeFloor(ALevelGenerator* owner, int xIndex, int yIndex)
{
	GeneratorOwner = owner;

	if (GeneratorOwner != nullptr)
	{
		GeneratorOwner->FloorCheckNeighbour.AddDynamic(this, &AGeneratedFloor::CheckNeighbours);
	}

	XIndex = xIndex;
	YIndex = yIndex;

	Dimensions.Z = 0.0f;
	SetActorLocation(GetActorLocation() * Dimensions);

	SetMainState();
}

void AGeneratedFloor::SetMainState()
{
	if (GeneratorOwner == nullptr)
		return;

	float random = FMath::RandRange(0.0f, 100.0f);

	float percentage = GeneratorOwner->GetFullPercentage();

	if (random >= percentage)
	{
		MainState = EMainFloorState::Blank;
	}
	else
	{
		MainState = EMainFloorState::Full;
	}

}

void AGeneratedFloor::CheckNeighbours()
{
	if (GeneratorOwner == nullptr)
		return;

	CheckHorizontalNeighbour();
	CheckVerticalNeighbour();
	SetFloorShape();

	if (MainState == EMainFloorState::Blank)
	{
		DebugUpArrow->SetArrowColor(FLinearColor(0.0f, 0.0f, 0.0f));
		DebugDownArrow->SetArrowColor(FLinearColor(0.0f, 0.0f, 0.0f));
		DebugLeftArrow->SetArrowColor(FLinearColor(0.0f, 0.0f, 0.0f));
		DebugRightArrow->SetArrowColor(FLinearColor(0.0f, 0.0f, 0.0f));
	}

}

void AGeneratedFloor::SetFloorShape()
{
	switch (HorizontalShape)
	{
	case EHorizontalFloorShape::Left:
		DebugLeftArrow->SetArrowColor(FLinearColor(0.0f, 1.0f, 0.0f));
		DebugRightArrow->SetArrowColor(FLinearColor(1.0f, 0.0f, 0.0f));
		break;
	case EHorizontalFloorShape::Right:
		DebugLeftArrow->SetArrowColor(FLinearColor(1.0f, 0.0f, 0.0f));
		DebugRightArrow->SetArrowColor(FLinearColor(0.0f, 1.0f, 0.0f));
		break;
	case EHorizontalFloorShape::Full:
		DebugLeftArrow->SetArrowColor(FLinearColor(0.0f, 1.0f, 0.0f));
		DebugRightArrow->SetArrowColor(FLinearColor(0.0f, 1.0f, 0.0f));
		break;
	default:
		DebugLeftArrow->SetArrowColor(FLinearColor(1.0f, 0.0f, 0.0f));
		DebugRightArrow->SetArrowColor(FLinearColor(1.0f, 0.0f, 0.0f));
		break;
	}

	switch (VerticalShape)
	{
	case EVerticalFloorShape::Up:
		DebugUpArrow->SetArrowColor(FLinearColor(0.0f, 1.0f, 0.0f));
		DebugDownArrow->SetArrowColor(FLinearColor(1.0f, 0.0f, 0.0f));
		break;
	case EVerticalFloorShape::Down:
		DebugUpArrow->SetArrowColor(FLinearColor(1.0f, 0.0f, 0.0f));
		DebugDownArrow->SetArrowColor(FLinearColor(0.0f, 1.0f, 0.0f));
		break;
	case EVerticalFloorShape::Full:
		DebugUpArrow->SetArrowColor(FLinearColor(0.0f, 1.0f, 0.0f));
		DebugDownArrow->SetArrowColor(FLinearColor(0.0f, 1.0f, 0.0f));
		break;
	default:
		DebugUpArrow->SetArrowColor(FLinearColor(1.0f, 0.0f, 0.0f));
		DebugDownArrow->SetArrowColor(FLinearColor(1.0f, 0.0f, 0.0f));
		break;
	}
}

void AGeneratedFloor::CheckHorizontalNeighbour()
{
	int index = XIndex + (GeneratorOwner->GetMapWidth() * YIndex);

	if (XIndex > 0.0f)
	{
		AGeneratedFloor* neighbourFloor = GeneratorOwner->GetGeneratedFloorAtIndex(index - 1);
		if (neighbourFloor->GetMainState() == EMainFloorState::Full)
		{
			HorizontalShape = EHorizontalFloorShape::Left;
		}
	}

	if (XIndex < GeneratorOwner->GetMapWidth()-1)
	{
		AGeneratedFloor* neighbourFloor = GeneratorOwner->GetGeneratedFloorAtIndex(index + 1);
		if (neighbourFloor->GetMainState() == EMainFloorState::Full)
		{
			if (HorizontalShape == EHorizontalFloorShape::Left)
			{
				HorizontalShape = EHorizontalFloorShape::Full;
			}
			else if (HorizontalShape == EHorizontalFloorShape::Blank)
			{
				HorizontalShape = EHorizontalFloorShape::Right;
			}
		}
	}
}

void AGeneratedFloor::CheckVerticalNeighbour()
{
	int index = XIndex + (GeneratorOwner->GetMapWidth() * YIndex);

	if (YIndex > 0.0f)
	{
		AGeneratedFloor* neighbourFloor = GeneratorOwner->GetGeneratedFloorAtIndex(index - GeneratorOwner->GetMapWidth());
		if (neighbourFloor->GetMainState() == EMainFloorState::Full)
		{
			VerticalShape = EVerticalFloorShape::Up;
		}
	}

	if (YIndex < GeneratorOwner->GetMapDepth()-1)
	{
		AGeneratedFloor* neighbourFloor = GeneratorOwner->GetGeneratedFloorAtIndex(index + GeneratorOwner->GetMapWidth());
		if (neighbourFloor->GetMainState() == EMainFloorState::Full)
		{
			if (VerticalShape == EVerticalFloorShape::Up)
			{
				VerticalShape = EVerticalFloorShape::Full;
			}
			else if (VerticalShape == EVerticalFloorShape::Blank)
			{
				VerticalShape = EVerticalFloorShape::Down;
			}
		}
	}
}