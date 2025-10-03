// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelGenerator.h"
#include "GeneratedFloor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ALevelGenerator::ALevelGenerator() :
	FullPiece(nullptr),
	SidePiece(nullptr),
	ConcaveCornerPiece(nullptr),
	ConvexCornerPiece(nullptr),
	SpawnWidth(1),
	SpawnDepth(1),
	FullPercentage(75.0f)

{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	FullPiece = CreateDefaultSubobject<UStaticMeshComponent>("FullPiece");
	FullPiece->SetupAttachment(RootComponent);

	SidePiece = CreateDefaultSubobject<UStaticMeshComponent>("SidePiece");
	SidePiece->SetupAttachment(RootComponent);

	ConcaveCornerPiece = CreateDefaultSubobject<UStaticMeshComponent>("ConcaveCornerPiece");
	ConcaveCornerPiece->SetupAttachment(RootComponent);

	ConvexCornerPiece = CreateDefaultSubobject<UStaticMeshComponent>("ConvexCornerPiece");
	ConvexCornerPiece->SetupAttachment(RootComponent);

	SetReplicates(true);
	bAlwaysRelevant = true;
}

// Called when the game starts or when spawned
void ALevelGenerator::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		for (int y = 0; y < SpawnDepth; y++)
		{
			for (int x = 0; x < SpawnWidth; x++)
			{
				MC_GenerateFloor(x, y);
			}
		}

		for (int y = 0; y < SpawnDepth; y++)
		{
			for (int x = 0; x < SpawnWidth; x++)
			{
				MC_CheckFloor(x, y);
			}
		}
		FloorValuesArray = FloorValuesArray;
	}

	MC_CreateFloor();
}

void ALevelGenerator::OnRep_SetRandomValue()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Green, FString::Printf(TEXT("Random: %f"), Random));
	}
	FloorValues newFloorValues;
}

void ALevelGenerator::OnRep_SetFloorValue()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Green, FString::Printf(TEXT("Floor Value: %f"), FloorValuesArray.Num()));
	}

}

void ALevelGenerator::OnRep_SetFullValue()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Green, FString::Printf(TEXT("Full: %f"), FullFloorPieceTransforms.Num()));
	}

}

void ALevelGenerator::OnRep_SetSideValue()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Green, FString::Printf(TEXT("Side: %f"), SideFloorPieceTransforms.Num()));
	}

}

void ALevelGenerator::OnRep_SetConcaveValue()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Green, FString::Printf(TEXT("Concave: %f"), ConcaveCornerPieceTransforms.Num()));
	}

}

void ALevelGenerator::OnRep_SetConvexValue()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Green, FString::Printf(TEXT("Convex: %f"), ConvexCornerTransforms.Num()));
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

void ALevelGenerator::MC_GenerateFloor_Implementation(int x, int y)
{
	GenerateFloor(x, y);
}

void ALevelGenerator::InitializeFloor()
{
	Random = FMath::RandRange(0.0f, 100.0f);
	FloorValues newFloorValues;

	if (Random > FullPercentage)
	{
		newFloorValues.IsFullTile = false;
	}
	else
	{
		newFloorValues.IsFullTile = true;
	}
	FloorValuesArray.Add(newFloorValues);
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

void ALevelGenerator::MC_CheckFloor_Implementation(int x, int y)
{
	CheckFloor(x, y);
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

void ALevelGenerator::ForceFloorBool(bool forcedFloor, int x, int y)
{
	int index = x + (y * SpawnWidth);

	FloorValuesArray[index].IsFullTile = true;
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
		ISMComp->RegisterComponent();

		if (meshArray[i]->GetStaticMesh() != nullptr)
		{
			ISMComp->SetStaticMesh(meshArray[i]->GetStaticMesh());
		}
		if (meshArray[i]->GetMaterial(0) != nullptr)
		{
			ISMComp->SetMaterial(0, meshArray[i]->GetMaterial(0));
		}

		ISMComp->SetFlags(RF_Transactional);
		this->AddInstanceComponent(ISMComp);

		ISMComp->AddInstances(arrayOfTransformArrays[i], true);
	}

}

void ALevelGenerator::MC_CreateFloor_Implementation()
{
	CreateFloor();
}

void ALevelGenerator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALevelGenerator, Random);
	DOREPLIFETIME(ALevelGenerator, FloorValuesArray);
	DOREPLIFETIME(ALevelGenerator, FullFloorPieceTransforms);
	DOREPLIFETIME(ALevelGenerator, SideFloorPieceTransforms);
	DOREPLIFETIME(ALevelGenerator, ConcaveCornerPieceTransforms);
	DOREPLIFETIME(ALevelGenerator, ConvexCornerTransforms);
}