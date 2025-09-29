// Fill out your copyright notice in the Description page of Project Settings.


#include "GeneratedFloor.h"
#include "LevelGenerator.h"
#include "Components/SceneComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/TextRenderComponent.h"

// Sets default values
AGeneratedFloor::AGeneratedFloor() :
	Origin(nullptr),
	GeneratorOwner(nullptr),
	MainState(EMainFloorState::Blank),
	FloorNeighbours(EFloorNeighbour::Blank)
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

	DebugUpLeftArrow = CreateDefaultSubobject<UArrowComponent>("UpLeftArrow");
	DebugUpLeftArrow->SetupAttachment(Origin);
	DebugUpLeftArrow->SetArrowColor(FLinearColor(0.0f, 0.0f, 0.0f));

	DebugUpRightArrow = CreateDefaultSubobject<UArrowComponent>("UpRightArrow");
	DebugUpRightArrow->SetupAttachment(Origin);
	DebugUpRightArrow->SetArrowColor(FLinearColor(0.0f, 0.0f, 0.0f));

	DebugDownLeftArrow = CreateDefaultSubobject<UArrowComponent>("DownLeftArrow");
	DebugDownLeftArrow->SetupAttachment(Origin);
	DebugDownLeftArrow->SetArrowColor(FLinearColor(0.0f, 0.0f, 0.0f));

	DebugDownRightArrow = CreateDefaultSubobject<UArrowComponent>("DownRightArrow");
	DebugDownRightArrow->SetupAttachment(Origin);
	DebugDownRightArrow->SetArrowColor(FLinearColor(0.0f, 0.0f, 0.0f));

	UpLeftCornerText = CreateDefaultSubobject<UTextRenderComponent>("UpLeftCornerText");
	UpLeftCornerText->SetupAttachment(Origin);

	DownLeftCornerText = CreateDefaultSubobject<UTextRenderComponent>("DownLeftCornerText");
	DownLeftCornerText->SetupAttachment(Origin);

	UpRightCornerText = CreateDefaultSubobject<UTextRenderComponent>("UpRightCornerText");
	UpRightCornerText->SetupAttachment(Origin);

	DownRightCornerText = CreateDefaultSubobject<UTextRenderComponent>("DownRightCornerText");
	DownRightCornerText->SetupAttachment(Origin);

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

void AGeneratedFloor::ForceFloorState(bool forceState)
{
	forceState ? 
		MainState = EMainFloorState::Full :
		MainState = EMainFloorState::Blank;
}

void AGeneratedFloor::SetMainState()
{
	if (GeneratorOwner == nullptr)
		return;

	float random = FMath::RandRange(0.0f, 100.0f);

	float percentage = GeneratorOwner->GetFullPercentage();

	if (random > percentage)
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

	bool checkLeft = XIndex > 0.0f;
	bool checkRight = XIndex < GeneratorOwner->GetMapWidth() - 1;

	bool checkUp = YIndex > 0.0f;
	bool checkDown = YIndex < GeneratorOwner->GetMapDepth() - 1;

	if (checkUp)
	{
		CheckTopNeighbours(checkLeft, checkRight, -GeneratorOwner->GetMapWidth());
	}

		CheckMiddleNeighbours(checkLeft, checkRight, 0);

	if (checkDown)
	{
		CheckDownNeighbours(checkLeft, checkRight, GeneratorOwner->GetMapWidth());
	}

	SetFloorShape();
}

void AGeneratedFloor::CheckTopNeighbours(bool checkLeft, bool checkRight, int indexOffset)
{
	int index = (XIndex + (GeneratorOwner->GetMapWidth() * YIndex)) + indexOffset;

	if (checkLeft)
	{
		AGeneratedFloor* leftNeighbourFloor = GeneratorOwner->GetGeneratedFloorAtIndex(index - 1);
		if (leftNeighbourFloor->GetMainState() == EMainFloorState::Full)
		{
			FloorNeighbours |= EFloorNeighbour::TopLeft;
		}
		else if (leftNeighbourFloor->GetMainState() == EMainFloorState::Blank)
		{
			FloorNeighbours &= ~EFloorNeighbour::TopLeft;
		}
	}

	AGeneratedFloor* middleNeighbourFloor = GeneratorOwner->GetGeneratedFloorAtIndex(index);
	if (middleNeighbourFloor->GetMainState() == EMainFloorState::Full)
	{
		FloorNeighbours |= EFloorNeighbour::TopMiddle;
	}
	else if (middleNeighbourFloor->GetMainState() == EMainFloorState::Blank)
	{
		FloorNeighbours &= ~EFloorNeighbour::TopMiddle;
	}

	if (checkRight)
	{
		AGeneratedFloor* rightNeighbourFloor = GeneratorOwner->GetGeneratedFloorAtIndex(index + 1);
		if (rightNeighbourFloor->GetMainState() == EMainFloorState::Full)
		{
			FloorNeighbours |= EFloorNeighbour::TopRight;
		}
		else if (rightNeighbourFloor->GetMainState() == EMainFloorState::Blank)
		{
			FloorNeighbours &= ~EFloorNeighbour::TopRight;
		}
	}
}

void AGeneratedFloor::CheckMiddleNeighbours(bool checkLeft, bool checkRight, int indexOffset)
{
	int index = (XIndex + (GeneratorOwner->GetMapWidth() * YIndex)) + indexOffset;

	if (checkLeft)
	{
		AGeneratedFloor* leftNeighbourFloor = GeneratorOwner->GetGeneratedFloorAtIndex(index - 1);
		if (leftNeighbourFloor->GetMainState() == EMainFloorState::Full)
		{
			FloorNeighbours |= EFloorNeighbour::MiddleLeft;
		}
		else if (leftNeighbourFloor->GetMainState() == EMainFloorState::Blank)
		{
			FloorNeighbours &= ~EFloorNeighbour::MiddleLeft;
		}
	}

	if (checkRight)
	{
		AGeneratedFloor* rightNeighbourFloor = GeneratorOwner->GetGeneratedFloorAtIndex(index + 1);
		if (rightNeighbourFloor->GetMainState() == EMainFloorState::Full)
		{
			FloorNeighbours |= EFloorNeighbour::MiddleRight;
		}
		else if (rightNeighbourFloor->GetMainState() == EMainFloorState::Blank)
		{
			FloorNeighbours &= ~EFloorNeighbour::MiddleRight;
		}
	}

}

void AGeneratedFloor::CheckDownNeighbours(bool checkLeft, bool checkRight, int indexOffset)
{
	int index = (XIndex + (GeneratorOwner->GetMapWidth() * YIndex)) + indexOffset;

	if (checkLeft)
	{
		AGeneratedFloor* leftNeighbourFloor = GeneratorOwner->GetGeneratedFloorAtIndex(index - 1);
		if (leftNeighbourFloor->GetMainState() == EMainFloorState::Full)
		{
			FloorNeighbours |= EFloorNeighbour::BottomLeft;
		}
		else if (leftNeighbourFloor->GetMainState() == EMainFloorState::Blank)
		{
			FloorNeighbours &= ~EFloorNeighbour::BottomLeft;
		}
	}

	AGeneratedFloor* middleNeighbourFloor = GeneratorOwner->GetGeneratedFloorAtIndex(index);
	if (middleNeighbourFloor->GetMainState() == EMainFloorState::Full)
	{
		FloorNeighbours |= EFloorNeighbour::BottomMiddle;
	}
	else if (middleNeighbourFloor->GetMainState() == EMainFloorState::Blank)
	{
		FloorNeighbours &= ~EFloorNeighbour::BottomMiddle;
	}

	if (checkRight)
	{
		AGeneratedFloor* rightNeighbourFloor = GeneratorOwner->GetGeneratedFloorAtIndex(index + 1);
		if (rightNeighbourFloor->GetMainState() == EMainFloorState::Full)
		{
			FloorNeighbours |= EFloorNeighbour::BottomRight;
		}
		else if (rightNeighbourFloor->GetMainState() == EMainFloorState::Blank)
		{
			FloorNeighbours &= ~EFloorNeighbour::BottomRight;
		}
	}

}

void AGeneratedFloor::SetFloorShape()
{
	TArray<UArrowComponent*> arrowArray;
	arrowArray.Add(DebugUpLeftArrow);
	arrowArray.Add(DebugUpArrow);
	arrowArray.Add(DebugUpRightArrow);
	arrowArray.Add(DebugLeftArrow);
	arrowArray.Add(DebugRightArrow);
	arrowArray.Add(DebugDownLeftArrow);
	arrowArray.Add(DebugDownArrow);
	arrowArray.Add(DebugDownRightArrow);

	TArray<UTextRenderComponent*> textRenderArray;
	textRenderArray.Add(UpLeftCornerText);
	textRenderArray.Add(UpRightCornerText);
	textRenderArray.Add(DownLeftCornerText);
	textRenderArray.Add(DownRightCornerText);



	for (int i = 0; i < arrowArray.Num(); i++)
	{
		if (int(FloorNeighbours) & (1 << i))
		{
			MainState == EMainFloorState::Full ?
				arrowArray[i]->SetArrowColor(FLinearColor(0.0f, 1.0f, 0.0f)) :
				arrowArray[i]->SetArrowColor(FLinearColor(0.0f, 0.0f, 1.0f));
		}
		else
		{
			MainState == EMainFloorState::Full ?
				arrowArray[i]->SetArrowColor(FLinearColor(1.0f, 0.0f, 0.0f)) :
				arrowArray[i]->SetArrowColor(FLinearColor(0.0f, 0.0f, 0.0f));
		}
	}

	SetTopLeftShape();
	SetTopRightShape();
	SetBottomLeftShape();
	SetBottomRightShape();

	/*
	* Each Corner Type
	* -Full
	* -Side (Must be rotated properly)
	* -Convex Corner (Must be rotated properly)
	* -Concave Corner (Must be rotated properly)
	*
	UpLeftCornerText
	UpRightCornerText
	DownLeftCornerText
	DownRightCornerText
	*/
}

void AGeneratedFloor::SetTopLeftShape()
{
	EFloorNeighbour topLeftCornerCheck = FloorNeighbours & (EFloorNeighbour::TopLeft | EFloorNeighbour::TopMiddle | EFloorNeighbour::MiddleLeft);

	FString topLeftString = FString("None");

	switch (topLeftCornerCheck)
	{
	case EFloorNeighbour::TopLeft:
		if (MainState == EMainFloorState::Full)
			topLeftString = FString("Full");
		break;
	case EFloorNeighbour::TopMiddle:
		if (MainState == EMainFloorState::Full)
			topLeftString = FString("Side Top");
		break;
	case EFloorNeighbour::MiddleLeft:
		if (MainState == EMainFloorState::Full)
			topLeftString = FString("Side Left");
		break;
	case EFloorNeighbour::TopLeft | EFloorNeighbour::TopMiddle:
		if (MainState == EMainFloorState::Full)
			topLeftString = FString("Full Top");
		break;
	case EFloorNeighbour::TopLeft | EFloorNeighbour::MiddleLeft:
		if (MainState == EMainFloorState::Full)
			topLeftString = FString("Full Left");
		break;
	case EFloorNeighbour::TopMiddle | EFloorNeighbour::MiddleLeft:
		if (MainState == EMainFloorState::Full)
		{
			topLeftString = FString("Full Top & Left");
		}
		else
		{
			topLeftString = FString("Concave Corner");
		}
		break;
	case EFloorNeighbour::TopLeft | EFloorNeighbour::TopMiddle | EFloorNeighbour::MiddleLeft:
		if (MainState == EMainFloorState::Full)
		{
			topLeftString = FString("True Full");
		}
		else
		{
			topLeftString = FString("Concave Corner Full");
		}
		break;
	default:
		if (MainState == EMainFloorState::Full)
		{
			topLeftString = FString("Convex Corner");
		}
		else
		{
			topLeftString = FString("None");
		}
		break;
	}

	UpLeftCornerText->SetText(FText::FromString(topLeftString));
}

void AGeneratedFloor::SetTopRightShape()
{
	EFloorNeighbour topRightCornerCheck = FloorNeighbours & (EFloorNeighbour::TopRight | EFloorNeighbour::TopMiddle | EFloorNeighbour::MiddleRight);

	FString topRightString = FString("None");

	switch (topRightCornerCheck)
	{
	case EFloorNeighbour::TopRight:
		if (MainState == EMainFloorState::Full)
			topRightString = FString("Full");
		break;
	case EFloorNeighbour::TopMiddle:
		if (MainState == EMainFloorState::Full)
			topRightString = FString("Side Top");
		break;
	case EFloorNeighbour::MiddleRight:
		if (MainState == EMainFloorState::Full)
			topRightString = FString("Side Right");
		break;
	case EFloorNeighbour::TopRight | EFloorNeighbour::TopMiddle:
		if (MainState == EMainFloorState::Full)
			topRightString = FString("Full Top");
		break;
	case EFloorNeighbour::TopRight | EFloorNeighbour::MiddleRight:
		if (MainState == EMainFloorState::Full)
			topRightString = FString("Full Right");
		break;
	case EFloorNeighbour::TopMiddle | EFloorNeighbour::MiddleRight:
		if (MainState == EMainFloorState::Full)
		{
			topRightString = FString("Full Top & Right");
		}
		else
		{
			topRightString = FString("Concave Corner");
		}
		break;
	case EFloorNeighbour::TopRight | EFloorNeighbour::TopMiddle | EFloorNeighbour::MiddleRight:
		if (MainState == EMainFloorState::Full)
		{
			topRightString = FString("True Full");
		}
		else
		{
			topRightString = FString("Concave Corner Full");
		}
		break;
	default:
		if (MainState == EMainFloorState::Full)
		{
			topRightString = FString("Convex Corner");
		}
		else
		{
			topRightString = FString("None");
		}
		break;
	}
	UpRightCornerText->SetText(FText::FromString(topRightString));

}

void AGeneratedFloor::SetBottomLeftShape()
{
	EFloorNeighbour bottomLeftCornerCheck = FloorNeighbours & (EFloorNeighbour::BottomLeft | EFloorNeighbour::BottomMiddle | EFloorNeighbour::MiddleLeft);

	FString bottomLeftString = FString("None");

	switch (bottomLeftCornerCheck)
	{
	case EFloorNeighbour::BottomLeft:
		if (MainState == EMainFloorState::Full)
			bottomLeftString = FString("Full");
		break;
	case EFloorNeighbour::BottomMiddle:
		if (MainState == EMainFloorState::Full)
			bottomLeftString = FString("Side Top");
		break;
	case EFloorNeighbour::MiddleLeft:
		if (MainState == EMainFloorState::Full)
			bottomLeftString = FString("Side Left");
		break;
	case EFloorNeighbour::BottomLeft | EFloorNeighbour::BottomMiddle:
		if (MainState == EMainFloorState::Full)
			bottomLeftString = FString("Full Bottom");
		break;
	case EFloorNeighbour::BottomLeft | EFloorNeighbour::MiddleLeft:
		if (MainState == EMainFloorState::Full)
			bottomLeftString = FString("Full Left");
		break;
	case EFloorNeighbour::BottomMiddle | EFloorNeighbour::MiddleLeft:
		if (MainState == EMainFloorState::Full)
		{
			bottomLeftString = FString("Full Bottom & Left");
		}
		else
		{
			bottomLeftString = FString("Concave Corner");
		}
		break;
	case EFloorNeighbour::BottomLeft | EFloorNeighbour::BottomMiddle | EFloorNeighbour::MiddleLeft:
		if (MainState == EMainFloorState::Full)
		{
			bottomLeftString = FString("True Full");
		}
		else
		{
			bottomLeftString = FString("Concave Corner Full");
		}
		break;
	default:
		if (MainState == EMainFloorState::Full)
		{
			bottomLeftString = FString("Convex Corner");
		}
		else
		{
			bottomLeftString = FString("None");
		}
		break;

	}

	DownLeftCornerText->SetText(FText::FromString(bottomLeftString));
}

void AGeneratedFloor::SetBottomRightShape()
{
	EFloorNeighbour bottomRightCornerCheck = FloorNeighbours & (EFloorNeighbour::BottomRight | EFloorNeighbour::BottomMiddle | EFloorNeighbour::MiddleRight);

	FString bottomRightString = FString("None");

	switch (bottomRightCornerCheck)
	{
	case EFloorNeighbour::BottomRight:
		if (MainState == EMainFloorState::Full)
			bottomRightString = FString("Full");
		break;
	case EFloorNeighbour::BottomMiddle:
		if (MainState == EMainFloorState::Full)
			bottomRightString = FString("Side Bottom");
		break;
	case EFloorNeighbour::MiddleRight:
		if (MainState == EMainFloorState::Full)
			bottomRightString = FString("Side Right");
		break;
	case EFloorNeighbour::BottomRight | EFloorNeighbour::BottomMiddle:
		if (MainState == EMainFloorState::Full)
			bottomRightString = FString("Full Bottom");
		break;
	case EFloorNeighbour::BottomRight | EFloorNeighbour::MiddleRight:
		if (MainState == EMainFloorState::Full)
			bottomRightString = FString("Full Right");
		break;
	case EFloorNeighbour::BottomMiddle | EFloorNeighbour::MiddleRight:
		if (MainState == EMainFloorState::Full)
		{
			bottomRightString = FString("Full Bottom & Right");
		}
		else
		{
			bottomRightString = FString("Concave Corner");
		}
		break;
	case EFloorNeighbour::BottomRight | EFloorNeighbour::BottomMiddle | EFloorNeighbour::MiddleRight:
		if (MainState == EMainFloorState::Full)
		{
			bottomRightString = FString("True Full");
		}
		else
		{
			bottomRightString = FString("Concave Corner Full");
		}
		break;
	default:
		if (MainState == EMainFloorState::Full)
		{
			bottomRightString = FString("Convex Corner");
		}
		else
		{
			bottomRightString = FString("None");
		}
		break;
	}
	DownRightCornerText->SetText(FText::FromString(bottomRightString));

}
