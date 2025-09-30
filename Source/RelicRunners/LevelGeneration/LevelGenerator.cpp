// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelGenerator.h"
#include "GeneratedFloor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ALevelGenerator::ALevelGenerator() :
	GeneratedFloorTemplate(nullptr),
	GeneratedObstacleTemplate(nullptr),
	FullPiece(nullptr),
	SidePiece(nullptr),
	ConcaveCornerPiece(nullptr),
	ConvexCornerPiece(nullptr),
	MeshComponent(nullptr),
	SpawnWidth(1),
	SpawnDepth(1),
	FullPercentage(75.0f)

{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ISMComp = CreateDefaultSubobject<UInstancedStaticMeshComponent>("Mesh");
	RootComponent = ISMComp;

	FullPiece = CreateDefaultSubobject<UInstancedStaticMeshComponent>("FullPiece");
	FullPiece->SetupAttachment(ISMComp);

	SidePiece = CreateDefaultSubobject<UInstancedStaticMeshComponent>("SidePiece");
	SidePiece->SetupAttachment(ISMComp);

	ConcaveCornerPiece = CreateDefaultSubobject<UInstancedStaticMeshComponent>("ConcaveCornerPiece");
	ConcaveCornerPiece->SetupAttachment(ISMComp);

	ConvexCornerPiece = CreateDefaultSubobject<UInstancedStaticMeshComponent>("ConvexCornerPiece");
	ConvexCornerPiece->SetupAttachment(ISMComp);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("RegularMesh");
	MeshComponent->SetupAttachment(ISMComp);

}

// Called when the game starts or when spawned
void ALevelGenerator::BeginPlay()
{
	Super::BeginPlay();

	TArray<FloorValues> floorArray;
	TArray<FTransform> fullFloorPieceTransforms;
	TArray<FTransform> sideFloorPieceTransforms;
	TArray<FTransform> concaveCornerPieceTransforms;
	TArray<FTransform> convexCornerTransforms;

	for (int y = 0; y < SpawnDepth; y++)
	{
		for (int x = 0; x < SpawnWidth; x++)
		{
			GenerateFloor(x, y, floorArray);
		}
	}

	for (int y = 0; y < SpawnDepth; y++)
	{
		for (int x = 0; x < SpawnWidth; x++)
		{
			CheckFloor(x, y, floorArray, fullFloorPieceTransforms, sideFloorPieceTransforms, concaveCornerPieceTransforms, convexCornerTransforms);
		}
	}

	if (FullPiece->GetStaticMesh() != nullptr)
		FullPiece->AddInstances(fullFloorPieceTransforms, true);

	if (SidePiece->GetStaticMesh() != nullptr)
		SidePiece->AddInstances(sideFloorPieceTransforms, true);

	if (ConcaveCornerPiece->GetStaticMesh() != nullptr)
		ConcaveCornerPiece->AddInstances(concaveCornerPieceTransforms, true);

	if (ConvexCornerPiece->GetStaticMesh() != nullptr)
		ConvexCornerPiece->AddInstances(convexCornerTransforms, true);
}

void ALevelGenerator::GenerateFloor(int x, int y, TArray<FloorValues>& floorValues)
{
	if (GeneratedFloorTemplate == nullptr)
		return;

	if (floorValues.Num() == x + (SpawnWidth * y))
	{
		FloorValues newFloorValues;
		floorValues.Add(newFloorValues);

		InitializeFloor(x, y, floorValues);

		if (CenterForceFull > 0)
		{
			if (((x - CenterForceFull) >= 0 && (x + CenterForceFull) <= SpawnWidth - 1)
				&& (y - CenterForceFull) >= 0 && (y + CenterForceFull) <= SpawnDepth - 1)
			{
				ForceFloorBool(true, x, y, floorValues);
			}
		}

		if (BorderForceFull > 0)
		{
			if (((x - BorderForceFull) < 0 || (x + BorderForceFull) > SpawnWidth - 1)
				|| (y - BorderForceFull) < 0 || (y + BorderForceFull) > SpawnDepth - 1)

			{
				ForceFloorBool(true, x, y, floorValues);
			}
		}
	}
}

void ALevelGenerator::InitializeFloor(int x, int y, TArray<FloorValues>& floorValues)
{
	float random = FMath::RandRange(0.0f, 100.0f);

	if (random > FullPercentage)
	{
		floorValues[x + (SpawnWidth * y)].IsFullTile = false;
	}
	else
	{
		floorValues[x + (SpawnWidth * y)].IsFullTile = true;
	}

}

void ALevelGenerator::CheckFloor(int x, int y, TArray<FloorValues>& floorValues, TArray<FTransform>& fullFloorPieceArray, TArray<FTransform>& sideFloorPieceArray, TArray<FTransform>& concaveCornerPieceArray, TArray<FTransform>& convexCornerArray)
{
	bool checkLeft = x > 0.0f;
	bool checkRight = x < SpawnWidth - 1;

	bool checkUp = y > 0.0f;
	bool checkDown = y < SpawnDepth - 1;

	if (checkUp)
	{
		FloorCheckTopNeighbours(checkLeft, checkRight, -SpawnWidth, x, y, floorValues);
	}

	FloorCheckMiddleNeighbours(checkLeft, checkRight, 0, x, y,floorValues);

	if (checkDown)
	{
		FloorCheckBottomNeighbours(checkLeft, checkRight, SpawnWidth, x, y, floorValues);
	}

	SetFloorShape(x, y, floorValues, fullFloorPieceArray, sideFloorPieceArray, concaveCornerPieceArray, convexCornerArray);

}

void ALevelGenerator::FloorCheckTopNeighbours(bool checkLeft, bool checkRight, int indexOffset, int x, int y, TArray<FloorValues>& floorValues)
{
	int indexToCheck = x + (y * SpawnWidth) + indexOffset;
	int index = x + (y * SpawnWidth);

	if (checkLeft)
	{
		if (floorValues[indexToCheck-1].IsFullTile)
		{
			floorValues[index].FloorNeighbours |= EFloorNeighbours::TopLeft;
		}
		else
		{
			floorValues[index].FloorNeighbours &= ~EFloorNeighbours::TopLeft;
		}
	}

	if (floorValues[indexToCheck].IsFullTile)
	{
		floorValues[index].FloorNeighbours |= EFloorNeighbours::TopMiddle;
	}
	else
	{
		floorValues[index].FloorNeighbours &= ~EFloorNeighbours::TopMiddle;
	}

	if (checkRight)
	{
		if (floorValues[indexToCheck+1].IsFullTile)
		{
			floorValues[index].FloorNeighbours |= EFloorNeighbours::TopRight;
		}
		else
		{
			floorValues[index].FloorNeighbours &= ~EFloorNeighbours::TopRight;
		}
	}
}

void ALevelGenerator::FloorCheckMiddleNeighbours(bool checkLeft, bool checkRight, int indexOffset, int x, int y, TArray<FloorValues>& floorValues)
{
	int indexToCheck = x + (y * SpawnWidth) + indexOffset;
	int index = x + (y * SpawnWidth);

	if (checkLeft)
	{
		if (floorValues[indexToCheck-1].IsFullTile)
		{
			floorValues[index].FloorNeighbours |= EFloorNeighbours::MiddleLeft;
		}
		else
		{
			floorValues[index].FloorNeighbours &= ~EFloorNeighbours::MiddleLeft;
		}
	}

	if (checkRight)
	{
		if (floorValues[indexToCheck+1].IsFullTile)
		{
			floorValues[index].FloorNeighbours |= EFloorNeighbours::MiddleRight;
		}
		else
		{
			floorValues[index].FloorNeighbours &= ~EFloorNeighbours::MiddleRight;
		}
	}

}

void ALevelGenerator::FloorCheckBottomNeighbours(bool checkLeft, bool checkRight, int indexOffset, int x, int y, TArray<FloorValues>& floorValues)
{
	int indexToCheck = x + (y * SpawnWidth) + indexOffset;
	int index = x + (y * SpawnWidth);

	if (checkLeft)
	{
		if (floorValues[indexToCheck-1].IsFullTile)
		{
			floorValues[index].FloorNeighbours |= EFloorNeighbours::BottomLeft;
		}
		else
		{
			floorValues[index].FloorNeighbours &= ~EFloorNeighbours::BottomLeft;
		}
	}

	if (floorValues[indexToCheck].IsFullTile)
	{
		floorValues[index].FloorNeighbours |= EFloorNeighbours::BottomMiddle;
	}
	else
	{
		floorValues[index].FloorNeighbours &= ~EFloorNeighbours::BottomMiddle;
	}

	if (checkRight)
	{
		if (floorValues[indexToCheck+1].IsFullTile)
		{
			floorValues[index].FloorNeighbours |= EFloorNeighbours::BottomRight;
		}
		else
		{
			floorValues[index].FloorNeighbours &= ~EFloorNeighbours::BottomRight;
		}
	}
}

void ALevelGenerator::ForceFloorBool(bool forcedFloor, int x, int y, TArray<FloorValues> &floorValues)
{
	int index = x + (y * SpawnWidth);

	floorValues[index].IsFullTile = true;
}

void ALevelGenerator::SetFloorShape(int x, int y, TArray<FloorValues>& floorValues, TArray<FTransform>& fullFloorPieceArray, TArray<FTransform>& sideFloorPieceArray, TArray<FTransform>& concaveCornerPieceArray, TArray<FTransform>& convexCornerArray)
{
	SetTopLeftFloorShape(x, y, floorValues, fullFloorPieceArray, sideFloorPieceArray, concaveCornerPieceArray, convexCornerArray);
	SetTopRightFloorShape(x, y, floorValues, fullFloorPieceArray, sideFloorPieceArray, concaveCornerPieceArray, convexCornerArray);
	SetBottomLeftFloorShape(x, y, floorValues, fullFloorPieceArray, sideFloorPieceArray, concaveCornerPieceArray, convexCornerArray);
	SetBottomRightFloorShape(x, y, floorValues, fullFloorPieceArray, sideFloorPieceArray, concaveCornerPieceArray, convexCornerArray);
}

void ALevelGenerator::SetTopLeftFloorShape(int x, int y, TArray<FloorValues>& floorValues, TArray<FTransform>& fullFloorPieceArray, TArray<FTransform>& sideFloorPieceArray, TArray<FTransform>& concaveCornerPieceArray, TArray<FTransform>& convexCornerArray)
{
	int index = x + (y * SpawnWidth);
	FVector posOffset = FVector((x * 2), (y * 2), 0.0f) * 40.0f;
	FRotator rotationOffset = FRotator(0.0f, 0.0f, 90.0f);
	FVector scaleOffset = FVector(20.0f);
	FTransform newTransform;

	newTransform.SetLocation(posOffset);
	newTransform.SetRotation(rotationOffset.Quaternion());
	newTransform.SetScale3D(scaleOffset);

	EFloorNeighbours topLeftCornerCheck = floorValues[index].FloorNeighbours & (EFloorNeighbours::TopLeft | EFloorNeighbours::TopMiddle | EFloorNeighbours::MiddleLeft);

	if (floorValues[index].IsFullTile)
	{
		switch (topLeftCornerCheck)
		{
		case EFloorNeighbours::TopLeft:
			fullFloorPieceArray.Add(newTransform);
			break;
		case EFloorNeighbours::TopMiddle:
			sideFloorPieceArray.Add(newTransform);
			break;
		case EFloorNeighbours::MiddleLeft:
			rotationOffset.Yaw = 90.0f;
			newTransform.SetRotation(rotationOffset.Quaternion());
			sideFloorPieceArray.Add(newTransform);
			break;
		case EFloorNeighbours::TopLeft | EFloorNeighbours::TopMiddle:
			fullFloorPieceArray.Add(newTransform);
			break;
		case EFloorNeighbours::TopLeft | EFloorNeighbours::MiddleLeft:
			fullFloorPieceArray.Add(newTransform);
			break;
		case EFloorNeighbours::TopMiddle | EFloorNeighbours::MiddleLeft:
			fullFloorPieceArray.Add(newTransform);
			break;
		case EFloorNeighbours::TopLeft | EFloorNeighbours::TopMiddle | EFloorNeighbours::MiddleLeft:
			fullFloorPieceArray.Add(newTransform);
			break;
		default:
			convexCornerArray.Add(newTransform);
		}
	}
	else
	{
		rotationOffset.Yaw = -90.0f;
		newTransform.SetRotation(rotationOffset.Quaternion());
		switch (topLeftCornerCheck)
		{
		case EFloorNeighbours::TopMiddle | EFloorNeighbours::MiddleLeft:
			concaveCornerPieceArray.Add(newTransform);
			break;
		case EFloorNeighbours::TopLeft | EFloorNeighbours::TopMiddle | EFloorNeighbours::MiddleLeft:
			concaveCornerPieceArray.Add(newTransform);
			break;
		}
	}
}

void ALevelGenerator::SetTopRightFloorShape(int x, int y, TArray<FloorValues>& floorValues, TArray<FTransform>& fullFloorPieceArray, TArray<FTransform>& sideFloorPieceArray, TArray<FTransform>& concaveCornerPieceArray, TArray<FTransform>& convexCornerArray)
{
	int index = x + (y * SpawnWidth);
	FVector posOffset = FVector((x * 2) + 1.0f, (y * 2), 0.0f) * 40.0f;
	FRotator rotationOffset = FRotator(0.0f, 0.0f, 90.0f);
	FVector scaleOffset = FVector(20.0f);
	FTransform newTransform;

	newTransform.SetLocation(posOffset);
	newTransform.SetRotation(rotationOffset.Quaternion());
	newTransform.SetScale3D(scaleOffset);

	EFloorNeighbours topRightCornerCheck = floorValues[index].FloorNeighbours & (EFloorNeighbours::TopRight | EFloorNeighbours::TopMiddle | EFloorNeighbours::MiddleRight);

	if (floorValues[index].IsFullTile)
	{
		switch (topRightCornerCheck)
		{
		case EFloorNeighbours::TopRight:
			fullFloorPieceArray.Add(newTransform);

			break;
		case EFloorNeighbours::TopMiddle:
			rotationOffset.Yaw = 180.0f;
			newTransform.SetRotation(rotationOffset.Quaternion());
			sideFloorPieceArray.Add(newTransform);
			break;
		case EFloorNeighbours::MiddleRight:
			rotationOffset.Yaw = 90.0f;
			newTransform.SetRotation(rotationOffset.Quaternion());
			sideFloorPieceArray.Add(newTransform);
			break;
		case EFloorNeighbours::TopRight | EFloorNeighbours::TopMiddle:
			fullFloorPieceArray.Add(newTransform);

			break;
		case EFloorNeighbours::TopRight | EFloorNeighbours::MiddleRight:
			fullFloorPieceArray.Add(newTransform);

			break;
		case EFloorNeighbours::TopMiddle | EFloorNeighbours::MiddleRight:
			fullFloorPieceArray.Add(newTransform);
			break;
		case EFloorNeighbours::TopRight | EFloorNeighbours::TopMiddle | EFloorNeighbours::MiddleRight:
			fullFloorPieceArray.Add(newTransform);
			break;
		default:
			rotationOffset.Yaw = 90.0f;
			newTransform.SetRotation(rotationOffset.Quaternion());
			convexCornerArray.Add(newTransform);
			break;
		}

	}
	else
	{
		switch (topRightCornerCheck)
		{

		case EFloorNeighbours::TopMiddle | EFloorNeighbours::MiddleRight:
			concaveCornerPieceArray.Add(newTransform);
			break;
		case EFloorNeighbours::TopRight | EFloorNeighbours::TopMiddle | EFloorNeighbours::MiddleRight:
			concaveCornerPieceArray.Add(newTransform);
			break;

		}
	}
}

void ALevelGenerator::SetBottomLeftFloorShape(int x, int y, TArray<FloorValues>& floorValues, TArray<FTransform>& fullFloorPieceArray, TArray<FTransform>& sideFloorPieceArray, TArray<FTransform>& concaveCornerPieceArray, TArray<FTransform>& convexCornerArray)
{
	int index = x + (y * SpawnWidth);
	FVector posOffset = FVector((x * 2), (y * 2) + 1.0f, 0.0f) * 40.0f;
	FRotator rotationOffset = FRotator(0.0f, 0.0f, 90.0f);
	FVector scaleOffset = FVector(20.0f);
	FTransform newTransform;

	newTransform.SetLocation(posOffset);
	newTransform.SetRotation(rotationOffset.Quaternion());
	newTransform.SetScale3D(scaleOffset);

	EFloorNeighbours bottomLeftCornerCheck = floorValues[index].FloorNeighbours & (EFloorNeighbours::BottomLeft | EFloorNeighbours::BottomMiddle | EFloorNeighbours::MiddleLeft);

	if (floorValues[index].IsFullTile)
	{
		switch (bottomLeftCornerCheck)
		{
		case EFloorNeighbours::BottomLeft:
			fullFloorPieceArray.Add(newTransform);
			break;
		case EFloorNeighbours::BottomMiddle:
			sideFloorPieceArray.Add(newTransform);
			break;
		case EFloorNeighbours::MiddleLeft:
			rotationOffset.Yaw = -90.0f;
			newTransform.SetRotation(rotationOffset.Quaternion());
			sideFloorPieceArray.Add(newTransform);
			break;
		case EFloorNeighbours::BottomLeft | EFloorNeighbours::BottomMiddle:
			fullFloorPieceArray.Add(newTransform);
			break;
		case EFloorNeighbours::BottomLeft | EFloorNeighbours::MiddleLeft:
			fullFloorPieceArray.Add(newTransform);
			break;
		case EFloorNeighbours::BottomMiddle | EFloorNeighbours::MiddleLeft:
			fullFloorPieceArray.Add(newTransform);
			break;
		case EFloorNeighbours::BottomLeft | EFloorNeighbours::BottomMiddle | EFloorNeighbours::MiddleLeft:
			fullFloorPieceArray.Add(newTransform);
			break;
		default:
			rotationOffset.Yaw = -90.0f;
			newTransform.SetRotation(rotationOffset.Quaternion());
			convexCornerArray.Add(newTransform);
			break;
		}
	}
	else
	{
		rotationOffset.Yaw = 180.0f;
		newTransform.SetRotation(rotationOffset.Quaternion());
		switch (bottomLeftCornerCheck)
		{

		case EFloorNeighbours::BottomMiddle | EFloorNeighbours::MiddleLeft:
			concaveCornerPieceArray.Add(newTransform);
			break;
		case EFloorNeighbours::BottomLeft | EFloorNeighbours::BottomMiddle | EFloorNeighbours::MiddleLeft:
			concaveCornerPieceArray.Add(newTransform);
			break;

		}
	}
}

void ALevelGenerator::SetBottomRightFloorShape(int x, int y, TArray<FloorValues>& floorValues, TArray<FTransform>& fullFloorPieceArray, TArray<FTransform>& sideFloorPieceArray, TArray<FTransform>& concaveCornerPieceArray, TArray<FTransform>& convexCornerArray)
{
	int index = x + (y * SpawnWidth);
	FVector posOffset = FVector((x * 2) + 1.0f, (y * 2) + 1.0f, 0.0f) * 40.0f;
	FRotator rotationOffset = FRotator(0.0f, 0.0f, 90.0f);
	FVector scaleOffset = FVector(20.0f);
	FTransform newTransform;

	newTransform.SetLocation(posOffset);
	newTransform.SetRotation(rotationOffset.Quaternion());
	newTransform.SetScale3D(scaleOffset);

	EFloorNeighbours bottomRightCornerCheck = floorValues[index].FloorNeighbours & (EFloorNeighbours::BottomRight | EFloorNeighbours::BottomMiddle | EFloorNeighbours::MiddleRight);

	if (floorValues[index].IsFullTile)
	{
		switch (bottomRightCornerCheck)
		{
		case EFloorNeighbours::BottomRight:
			fullFloorPieceArray.Add(newTransform);
			break;
		case EFloorNeighbours::BottomMiddle:
			rotationOffset.Yaw = 180.0f;
			newTransform.SetRotation(rotationOffset.Quaternion());
			sideFloorPieceArray.Add(newTransform);
			break;
		case EFloorNeighbours::MiddleRight:
			rotationOffset.Yaw = -90.0f;
			newTransform.SetRotation(rotationOffset.Quaternion());
			sideFloorPieceArray.Add(newTransform);
			break;
		case EFloorNeighbours::BottomRight | EFloorNeighbours::BottomMiddle:
			fullFloorPieceArray.Add(newTransform);
			break;
		case EFloorNeighbours::BottomRight | EFloorNeighbours::MiddleRight:
			fullFloorPieceArray.Add(newTransform);
			break;
		case EFloorNeighbours::BottomMiddle | EFloorNeighbours::MiddleRight:
			fullFloorPieceArray.Add(newTransform);
			break;
		case EFloorNeighbours::BottomRight | EFloorNeighbours::BottomMiddle | EFloorNeighbours::MiddleRight:
			fullFloorPieceArray.Add(newTransform);
			break;
		default:
			rotationOffset.Yaw = 180.0f;
			newTransform.SetRotation(rotationOffset.Quaternion());
			convexCornerArray.Add(newTransform);
			break;
		}
	}
	else
	{
		rotationOffset.Yaw = 90.0f;
		newTransform.SetRotation(rotationOffset.Quaternion());
		switch (bottomRightCornerCheck)
		{
		case EFloorNeighbours::BottomMiddle | EFloorNeighbours::MiddleRight:
			concaveCornerPieceArray.Add(newTransform);
			break;
		case EFloorNeighbours::BottomRight | EFloorNeighbours::BottomMiddle | EFloorNeighbours::MiddleRight:
				concaveCornerPieceArray.Add(newTransform);
			break;
		}
	}
}
