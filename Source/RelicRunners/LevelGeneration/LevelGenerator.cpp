// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelGenerator.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "PackedLevelActor/PackedLevelActor.h"
#include "NavigationSystem.h"

/*
TODO:
Create "Key tiles" that create paths from the start tile like how it works with the ending tile
Make the navmesh work with the level generation
Teleport every player to the starting point when the level starts
*/

ALevelGenerator::ALevelGenerator() :
	LevelStartPackedLevel(nullptr),
	LevelEndPackedLevel(nullptr),
	FullPiece(nullptr),
	SidePiece(nullptr),
	ConcaveCornerPiece(nullptr),
	ConvexCornerPiece(nullptr),
	SpawnWidth(2),
	SpawnDepth(2),
	FullPercentage(75.0f),
	BasicObstaclePercentage(50.0f),
	CenterForceFull(0),
	BorderForceFull(0),
	TileScale(1.0f),
	MaxKeyTileAmount(1),
	MaxShopAmount(1)
{
	PrimaryActorTick.bCanEverTick = false;

	Origin = CreateDefaultSubobject<USceneComponent>("Origin");
	RootComponent = Origin;

	FullPiece = CreateDefaultSubobject<UStaticMeshComponent>("FullPiece");
	FullPiece->SetupAttachment(Origin);

	SidePiece = CreateDefaultSubobject<UStaticMeshComponent>("SidePiece");
	SidePiece->SetupAttachment(Origin);

	ConcaveCornerPiece = CreateDefaultSubobject<UStaticMeshComponent>("ConcaveCornerPiece");
	ConcaveCornerPiece->SetupAttachment(Origin);

	ConvexCornerPiece = CreateDefaultSubobject<UStaticMeshComponent>("ConvexCornerPiece");
	ConvexCornerPiece->SetupAttachment(Origin);

	SetReplicates(true);
	bAlwaysRelevant = true;
}

void ALevelGenerator::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (HasAuthority())
	{
		int startingIndex = FMath::RandRange(0, (SpawnDepth * SpawnWidth) - 1);
		int endingIndex = FMath::RandRange(0, (SpawnDepth * SpawnWidth) - 1);

		while (startingIndex == endingIndex)
		{
			startingIndex = FMath::RandRange(0, (SpawnDepth * SpawnWidth) - 1);
			endingIndex = FMath::RandRange(0, (SpawnDepth * SpawnWidth) - 1);
		}

		for (int y = 0; y < SpawnDepth; y++)
		{
			for (int x = 0; x < SpawnWidth; x++)
			{
				GenerateFloor(x, y);
			}
		}

		if (SpawnDepth * SpawnWidth > 1)
		{
			SetStartingAndEndingPoints(startingIndex, endingIndex);

			FindStartToEndPath(startingIndex, endingIndex);
		}

		for (int i = 0; i < MaxKeyTileAmount; i++)
		{
			int keyTileIndex = FMath::RandRange(0, (SpawnDepth * SpawnWidth) - 1);

			int whileLoopLimit = 0;

			while (FloorValuesArray[keyTileIndex].FloorObstacle > EFloorObstacle::Basic)
			{
				keyTileIndex = FMath::RandRange(0, (SpawnDepth * SpawnWidth) - 1);

				whileLoopLimit++;

				if (whileLoopLimit >= 10)
				{
					break;
				}
			}

			if (whileLoopLimit < 10)
			{
				SetKeyTile(keyTileIndex);

				FindStartToEndPath(startingIndex, keyTileIndex);
			}
		}

		/*
		TODO: 
		Make a for loop that takes the key tiles array and sets the indexes of the specific FloorValuesArray and
		overrides whatever's there with the special tile like how the starting and ending tiles are done.
		Also - Create a path from the starting tile to the specific tile based on the index in the for loop.
		*/

		for (int y = 0; y < SpawnDepth; y++)
		{
			for (int x = 0; x < SpawnWidth; x++)
			{
				CheckFloor(x, y);

				Server_SpawnFloorObstacles(x, y);
			}
		}
		CreateFloor();
	}


	


	
}

void ALevelGenerator::BeginPlay()
{
	Super::BeginPlay();

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (NavSys)
	{

		NavSys->Build();
	}
	
}


void ALevelGenerator::GenerateFloor(int x, int y)
{
	if (FloorValuesArray.Num() == x + (SpawnWidth * y))
	{
		InitializeFloor();

		if (CenterForceFull > 0)
		{
			if (((x - CenterForceFull) >= 0 && (x + CenterForceFull) <= SpawnWidth - 1)
				&& (y - CenterForceFull) >= 0 && (y + CenterForceFull) <= SpawnDepth - 1)
			{
				ForceFloorBool(true, x, y);
			}
		}

		if (BorderForceFull > 0)
		{
			if (((x - BorderForceFull) < 0 || (x + BorderForceFull) > SpawnWidth - 1)
				|| (y - BorderForceFull) < 0 || (y + BorderForceFull) > SpawnDepth - 1)

			{
				ForceFloorBool(true, x, y);
			}
		}
	}
}

void ALevelGenerator::InitializeFloor()
{
	FloorValues newFloorValues;
	float randomFloor = FMath::RandRange(0.1f, 100.0f);

	if (randomFloor > FullPercentage)
	{
		newFloorValues.IsFullTile = false;
	}
	else
	{
		newFloorValues.IsFullTile = true;

		InitializeModularObstacle(newFloorValues);
	}
	FloorValuesArray.Add(newFloorValues);
}

void ALevelGenerator::ForceFloorBool(bool forcedFloor, int x, int y)
{
	int index = x + (y * SpawnWidth);

	FloorValuesArray[index].IsFullTile = forcedFloor;

	if (forcedFloor)
	{
		if (FloorValuesArray[index].FloorObstacle == EFloorObstacle::None)
		{
			InitializeModularObstacle(FloorValuesArray[index]);
		}
	}
}

void ALevelGenerator::SetStartingAndEndingPoints(int startingIndex, int endingIndex)
{
	FloorValuesArray[startingIndex].IsFullTile = true;
	FloorValuesArray[startingIndex].FloorObstacle = EFloorObstacle::Start;
	FloorValuesArray[startingIndex].randomFloorToSpawn = 0;
	FloorValuesArray[startingIndex].obstacleYaw = FMath::RandRange(0, 3) * 90;


	FloorValuesArray[endingIndex].IsFullTile = true;
	FloorValuesArray[endingIndex].FloorObstacle = EFloorObstacle::End;
	FloorValuesArray[endingIndex].randomFloorToSpawn = 0;
	FloorValuesArray[endingIndex].obstacleYaw = FMath::RandRange(0, 3) * 90;
}

void ALevelGenerator::SetKeyTile(int index)
{
	FloorValuesArray[index].IsFullTile = true;
	FloorValuesArray[index].FloorObstacle = EFloorObstacle::KeyTile;
	FloorValuesArray[index].randomFloorToSpawn = 0;
	FloorValuesArray[index].obstacleYaw = FMath::RandRange(0, 3) * 90;

}

void ALevelGenerator::FindStartToEndPath(int startingIndex, int targetIndex)
{
	int currentX = startingIndex % SpawnWidth;
	int currentY = startingIndex / SpawnWidth;

	int targetX = targetIndex % SpawnWidth;
	int targetY = targetIndex / SpawnWidth;

	bool bReachEndX = currentX == targetX;

	bool bReachEndY = currentY == targetY;

	while (!(bReachEndX && bReachEndY))
	{
		float random = FMath::FRandRange(0.0f, 100.0f);

		if (random >= 50.0f)
		{
			if (currentX > targetX)
			{
				currentX--;
			}
			else if (currentX < targetX)
			{
				currentX++;
			}
		}
		else
		{
			if (currentY > targetY)
			{
				currentY--;
			}
			else if (currentY < targetY)
			{
				currentY++;
			}
		}
		ForceFloorBool(true, currentX, currentY);

		bReachEndX = currentX == targetX;
		bReachEndY = currentY == targetY;
	}
}

void ALevelGenerator::InitializeModularObstacle(FloorValues& floorValue)
{
	float randomObstaclePerc = FMath::RandRange(0.1f, 100.0f);
	if (randomObstaclePerc < BasicObstaclePercentage)
	{
		floorValue.FloorObstacle = EFloorObstacle::Basic;

		floorValue.randomFloorToSpawn = FMath::RandRange(0, PackedLevelArray.Num() - 1);

		floorValue.obstacleYaw = FMath::RandRange(0, 3) * 90;
	}
}

void ALevelGenerator::CheckFloor(int x, int y)
{
	bool checkLeft = x > 0.0f;
	bool checkRight = x < SpawnWidth - 1;

	bool checkUp = y > 0.0f;
	bool checkDown = y < SpawnDepth - 1;

	if (checkUp)
	{
		FloorCheckTopNeighbours(checkLeft, checkRight, -SpawnWidth, x, y);
	}

	FloorCheckMiddleNeighbours(checkLeft, checkRight, 0, x, y);

	if (checkDown)
	{
		FloorCheckBottomNeighbours(checkLeft, checkRight, SpawnWidth, x, y);
	}

	SetFloorShape(x, y);

}

void ALevelGenerator::FloorCheckTopNeighbours(bool checkLeft, bool checkRight, int indexOffset, int x, int y)
{
	int indexToCheck = x + (y * SpawnWidth) + indexOffset;
	int index = x + (y * SpawnWidth);

	if (checkLeft)
	{
		if (FloorValuesArray[indexToCheck-1].IsFullTile)
		{
			FloorValuesArray[index].FloorNeighbours |= EFloorNeighbours::TopLeft;
		}
		else
		{
			FloorValuesArray[index].FloorNeighbours &= ~EFloorNeighbours::TopLeft;
		}
	}

	if (FloorValuesArray[indexToCheck].IsFullTile)
	{
		FloorValuesArray[index].FloorNeighbours |= EFloorNeighbours::TopMiddle;
	}
	else
	{
		FloorValuesArray[index].FloorNeighbours &= ~EFloorNeighbours::TopMiddle;
	}

	if (checkRight)
	{
		if (FloorValuesArray[indexToCheck + 1].IsFullTile)
		{
			FloorValuesArray[index].FloorNeighbours |= EFloorNeighbours::TopRight;
		}
		else
		{
			FloorValuesArray[index].FloorNeighbours &= ~EFloorNeighbours::TopRight;
		}
	}
}

void ALevelGenerator::FloorCheckMiddleNeighbours(bool checkLeft, bool checkRight, int indexOffset, int x, int y)
{
	int indexToCheck = x + (y * SpawnWidth) + indexOffset;
	int index = x + (y * SpawnWidth);

	if (checkLeft)
	{
		if (FloorValuesArray[indexToCheck - 1].IsFullTile)
		{
			FloorValuesArray[index].FloorNeighbours |= EFloorNeighbours::MiddleLeft;
		}
		else
		{
			FloorValuesArray[index].FloorNeighbours &= ~EFloorNeighbours::MiddleLeft;
		}
	}

	if (checkRight)
	{
		if (FloorValuesArray[indexToCheck + 1].IsFullTile)
		{
			FloorValuesArray[index].FloorNeighbours |= EFloorNeighbours::MiddleRight;
		}
		else
		{
			FloorValuesArray[index].FloorNeighbours &= ~EFloorNeighbours::MiddleRight;
		}
	}

}

void ALevelGenerator::FloorCheckBottomNeighbours(bool checkLeft, bool checkRight, int indexOffset, int x, int y)
{
	int indexToCheck = x + (y * SpawnWidth) + indexOffset;
	int index = x + (y * SpawnWidth);

	if (checkLeft)
	{
		if (FloorValuesArray[indexToCheck - 1].IsFullTile)
		{
			FloorValuesArray[index].FloorNeighbours |= EFloorNeighbours::BottomLeft;
		}
		else
		{
			FloorValuesArray[index].FloorNeighbours &= ~EFloorNeighbours::BottomLeft;
		}
	}

	if (FloorValuesArray[indexToCheck].IsFullTile)
	{
		FloorValuesArray[index].FloorNeighbours |= EFloorNeighbours::BottomMiddle;
	}
	else
	{
		FloorValuesArray[index].FloorNeighbours &= ~EFloorNeighbours::BottomMiddle;
	}

	if (checkRight)
	{
		if (FloorValuesArray[indexToCheck + 1].IsFullTile)
		{
			FloorValuesArray[index].FloorNeighbours |= EFloorNeighbours::BottomRight;
		}
		else
		{
			FloorValuesArray[index].FloorNeighbours &= ~EFloorNeighbours::BottomRight;
		}
	}
}

void ALevelGenerator::SetFloorShape(int x, int y)
{
	SetTopLeftFloorShape(x, y);
	SetTopRightFloorShape(x, y);
	SetBottomLeftFloorShape(x, y);
	SetBottomRightFloorShape(x, y);
}

void ALevelGenerator::SetTopLeftFloorShape(int x, int y)
{
	int index = x + (y * SpawnWidth);
	FVector posOffset = FVector((x * 2), (y * 2), 0.0f) * (TileScale * 2);
	FRotator rotationOffset = FRotator(0.0f, 0.0f, 0.0f);
	FTransform newTransform;

	newTransform.SetLocation(posOffset);
	newTransform.SetRotation(rotationOffset.Quaternion());

	EFloorNeighbours topLeftCornerCheck = FloorValuesArray[index].FloorNeighbours & (EFloorNeighbours::TopLeft | EFloorNeighbours::TopMiddle | EFloorNeighbours::MiddleLeft);

	if (FloorValuesArray[index].IsFullTile)
	{
		switch (topLeftCornerCheck)
		{
		case EFloorNeighbours::TopMiddle:
			SideFloorPieceTransforms.Add(newTransform);
			break;
		case EFloorNeighbours::MiddleLeft:
			rotationOffset.Yaw = 90.0f;
			newTransform.SetRotation(rotationOffset.Quaternion());
			SideFloorPieceTransforms.Add(newTransform);
			break;
		case EFloorNeighbours::TopLeft:
		case EFloorNeighbours::TopLeft | EFloorNeighbours::TopMiddle:
		case EFloorNeighbours::TopLeft | EFloorNeighbours::MiddleLeft:
		case EFloorNeighbours::TopMiddle | EFloorNeighbours::MiddleLeft:
		case EFloorNeighbours::TopLeft | EFloorNeighbours::TopMiddle | EFloorNeighbours::MiddleLeft:
			FullFloorPieceTransforms.Add(newTransform);
			break;
		default:
			ConvexCornerTransforms.Add(newTransform);
		}
	}
	else
	{
		rotationOffset.Yaw = -90.0f;
		newTransform.SetRotation(rotationOffset.Quaternion());
		switch (topLeftCornerCheck)
		{
		case EFloorNeighbours::TopMiddle | EFloorNeighbours::MiddleLeft:
		case EFloorNeighbours::TopLeft | EFloorNeighbours::TopMiddle | EFloorNeighbours::MiddleLeft:
			ConcaveCornerPieceTransforms.Add(newTransform);
			break;
		}
	}
}

void ALevelGenerator::SetTopRightFloorShape(int x, int y)
{
	int index = x + (y * SpawnWidth);
	FVector posOffset = FVector((x * 2) + 1.0f, (y * 2), 0.0f) * (TileScale * 2);
	FRotator rotationOffset = FRotator(0.0f, 0.0f, 0.0f);
	FTransform newTransform;

	newTransform.SetLocation(posOffset);
	newTransform.SetRotation(rotationOffset.Quaternion());

	EFloorNeighbours topRightCornerCheck = FloorValuesArray[index].FloorNeighbours & (EFloorNeighbours::TopRight | EFloorNeighbours::TopMiddle | EFloorNeighbours::MiddleRight);

	if (FloorValuesArray[index].IsFullTile)
	{
		switch (topRightCornerCheck)
		{
		case EFloorNeighbours::TopMiddle:
			rotationOffset.Yaw = 180.0f;
			newTransform.SetRotation(rotationOffset.Quaternion());
			SideFloorPieceTransforms.Add(newTransform);
			break;
		case EFloorNeighbours::MiddleRight:
			rotationOffset.Yaw = 90.0f;
			newTransform.SetRotation(rotationOffset.Quaternion());
			SideFloorPieceTransforms.Add(newTransform);
			break;
		case EFloorNeighbours::TopRight:
		case EFloorNeighbours::TopRight | EFloorNeighbours::TopMiddle:
		case EFloorNeighbours::TopRight | EFloorNeighbours::MiddleRight:
		case EFloorNeighbours::TopMiddle | EFloorNeighbours::MiddleRight:
		case EFloorNeighbours::TopRight | EFloorNeighbours::TopMiddle | EFloorNeighbours::MiddleRight:
			FullFloorPieceTransforms.Add(newTransform);
			break;
		default:
			rotationOffset.Yaw = 90.0f;
			newTransform.SetRotation(rotationOffset.Quaternion());
			ConvexCornerTransforms.Add(newTransform);
			break;
		}

	}
	else
	{
		switch (topRightCornerCheck)
		{

		case EFloorNeighbours::TopMiddle | EFloorNeighbours::MiddleRight:
		case EFloorNeighbours::TopRight | EFloorNeighbours::TopMiddle | EFloorNeighbours::MiddleRight:
			ConcaveCornerPieceTransforms.Add(newTransform);
			break;

		}
	}
}

void ALevelGenerator::SetBottomLeftFloorShape(int x, int y)
{
	int index = x + (y * SpawnWidth);
	FVector posOffset = FVector((x * 2), (y * 2) + 1.0f, 0.0f) * (TileScale * 2);
	FRotator rotationOffset = FRotator(0.0f, 0.0f, 0.0f);
	FTransform newTransform;

	newTransform.SetLocation(posOffset);
	newTransform.SetRotation(rotationOffset.Quaternion());

	EFloorNeighbours bottomLeftCornerCheck = FloorValuesArray[index].FloorNeighbours & (EFloorNeighbours::BottomLeft | EFloorNeighbours::BottomMiddle | EFloorNeighbours::MiddleLeft);

	if (FloorValuesArray[index].IsFullTile)
	{
		switch (bottomLeftCornerCheck)
		{
		case EFloorNeighbours::BottomMiddle:
			SideFloorPieceTransforms.Add(newTransform);
			break;
		case EFloorNeighbours::MiddleLeft:
			rotationOffset.Yaw = -90.0f;
			newTransform.SetRotation(rotationOffset.Quaternion());
			SideFloorPieceTransforms.Add(newTransform);
			break;
		case EFloorNeighbours::BottomLeft:
		case EFloorNeighbours::BottomLeft | EFloorNeighbours::BottomMiddle:
		case EFloorNeighbours::BottomLeft | EFloorNeighbours::MiddleLeft:
		case EFloorNeighbours::BottomMiddle | EFloorNeighbours::MiddleLeft:
		case EFloorNeighbours::BottomLeft | EFloorNeighbours::BottomMiddle | EFloorNeighbours::MiddleLeft:
			FullFloorPieceTransforms.Add(newTransform);
			break;
		default:
			rotationOffset.Yaw = -90.0f;
			newTransform.SetRotation(rotationOffset.Quaternion());
			ConvexCornerTransforms.Add(newTransform);
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
		case EFloorNeighbours::BottomLeft | EFloorNeighbours::BottomMiddle | EFloorNeighbours::MiddleLeft:
			ConcaveCornerPieceTransforms.Add(newTransform);
			break;

		}
	}
}

void ALevelGenerator::SetBottomRightFloorShape(int x, int y)
{
	int index = x + (y * SpawnWidth);
	FVector posOffset = FVector((x * 2) + 1.0f, (y * 2) + 1.0f, 0.0f) * (TileScale * 2);
	FRotator rotationOffset = FRotator(0.0f, 0.0f, 0.0f);
	FTransform newTransform;

	newTransform.SetLocation(posOffset);
	newTransform.SetRotation(rotationOffset.Quaternion());

	EFloorNeighbours bottomRightCornerCheck = FloorValuesArray[index].FloorNeighbours & (EFloorNeighbours::BottomRight | EFloorNeighbours::BottomMiddle | EFloorNeighbours::MiddleRight);

	if (FloorValuesArray[index].IsFullTile)
	{
		switch (bottomRightCornerCheck)
		{
		case EFloorNeighbours::BottomMiddle:
			rotationOffset.Yaw = 180.0f;
			newTransform.SetRotation(rotationOffset.Quaternion());
			SideFloorPieceTransforms.Add(newTransform);
			break;
		case EFloorNeighbours::MiddleRight:
			rotationOffset.Yaw = -90.0f;
			newTransform.SetRotation(rotationOffset.Quaternion());
			SideFloorPieceTransforms.Add(newTransform);
			break;
		case EFloorNeighbours::BottomRight:
		case EFloorNeighbours::BottomRight | EFloorNeighbours::BottomMiddle:
		case EFloorNeighbours::BottomRight | EFloorNeighbours::MiddleRight:
		case EFloorNeighbours::BottomMiddle | EFloorNeighbours::MiddleRight:
		case EFloorNeighbours::BottomRight | EFloorNeighbours::BottomMiddle | EFloorNeighbours::MiddleRight:
			FullFloorPieceTransforms.Add(newTransform);
			break;
		default:
			rotationOffset.Yaw = 180.0f;
			newTransform.SetRotation(rotationOffset.Quaternion());
			ConvexCornerTransforms.Add(newTransform);
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
		case EFloorNeighbours::BottomRight | EFloorNeighbours::BottomMiddle | EFloorNeighbours::MiddleRight:
			ConcaveCornerPieceTransforms.Add(newTransform);
			break;
		}
	}
}

void ALevelGenerator::Server_SpawnFloorObstacles_Implementation(int x, int y)
{
	SpawnFloorObstacles(x, y);
}

void ALevelGenerator::SpawnFloorObstacles(int x, int y)
{
	int index = x + (y * SpawnWidth);

	float yaw = FloorValuesArray[index].obstacleYaw;
	FVector posOffset = FVector((TileScale * x * 4) + TileScale, (TileScale * y * 4) + TileScale, TileScale);

	switch (FloorValuesArray[index].FloorObstacle)
	{
	case EFloorObstacle::Basic:
		if (PackedLevelArray[FloorValuesArray[index].randomFloorToSpawn] != nullptr)
		{
			APackedLevelActor* packed = GetWorld()->SpawnActor<APackedLevelActor>(PackedLevelArray[FloorValuesArray[index].randomFloorToSpawn], posOffset, FRotator(0.0f, yaw, 0.0f));
		}
		break;
	case EFloorObstacle::KeyTile:
		break;
	case EFloorObstacle::Shop:
		break;
	case EFloorObstacle::Start:
		if (LevelStartPackedLevel != nullptr)
		{
			APackedLevelActor* packed = GetWorld()->SpawnActor<APackedLevelActor>(LevelStartPackedLevel, posOffset, FRotator(0.0f, yaw, 0.0f));
		}
		break;
	case EFloorObstacle::End:
		if (LevelEndPackedLevel != nullptr)
		{
			APackedLevelActor* packed = GetWorld()->SpawnActor<APackedLevelActor>(LevelEndPackedLevel, posOffset, FRotator(0.0f, yaw, 0.0f));
		}
		break;
	}
}

void ALevelGenerator::CreateFloor()
{
	TArray<TArray<FTransform>> arrayOfTransformArrays;
	arrayOfTransformArrays.Add(FullFloorPieceTransforms);
	arrayOfTransformArrays.Add(SideFloorPieceTransforms);
	arrayOfTransformArrays.Add(ConcaveCornerPieceTransforms);
	arrayOfTransformArrays.Add(ConvexCornerTransforms);

	TArray<UStaticMeshComponent*> meshArray;
	meshArray.Add(FullPiece);
	meshArray.Add(SidePiece);
	meshArray.Add(ConcaveCornerPiece);
	meshArray.Add(ConvexCornerPiece);

	for (int i = 0; i < meshArray.Num(); i++)
	{
		UInstancedStaticMeshComponent* ISMComp = NewObject<UInstancedStaticMeshComponent>(this);
					

		if (meshArray[i]->GetStaticMesh() != nullptr)
		{
			ISMComp->SetStaticMesh(meshArray[i]->GetStaticMesh());
		}
		if (meshArray[i]->GetMaterial(0) != nullptr)
		{
			ISMComp->SetMaterial(0, meshArray[i]->GetMaterial(0));
		}

		ISMComp->AddInstances(arrayOfTransformArrays[i], false, true, true);

		ISMComp->RegisterComponent();
	
	}
}

void ALevelGenerator::OnRep_FloorValuesArrayChange()
{
	for (int y = 0; y < SpawnDepth; y++)
	{
		for (int x = 0; x < SpawnWidth; x++)
		{
			CheckFloor(x, y);
		}
	}
	CreateFloor();
}

void ALevelGenerator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALevelGenerator, FloorValuesArray);
}