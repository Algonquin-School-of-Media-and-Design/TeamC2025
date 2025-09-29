// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelGenerator.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFloorCheckNeighbour);

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EFloorNeighbours : uint8
{
	Blank = 0,
	TopLeft = 1 << 0,			/*0000 0001*/
	TopMiddle = 1 << 1,			/*0000 0010*/
	TopRight = 1 << 2,			/*0000 0100*/
	MiddleLeft = 1 << 3,		/*0000 1000*/
	MiddleRight = 1 << 4,		/*0001 0000*/
	BottomLeft = 1 << 5,		/*0010 0000*/
	BottomMiddle = 1 << 6,		/*0100 0000*/
	BottomRight = 1 << 7,		/*1000 0000*/
};
ENUM_CLASS_FLAGS(EFloorNeighbours);

struct FloorValues
{
public:
	bool IsFullTile = false;
	EFloorNeighbours FloorNeighbours = EFloorNeighbours::Blank;
};


UCLASS()
class RELICRUNNERS_API ALevelGenerator : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALevelGenerator();

	UPROPERTY(BlueprintAssignable)
	FFloorCheckNeighbour FloorCheckNeighbour;

	UPROPERTY(EditAnywhere, Category = "SpawningValues")
	TSubclassOf<class AGeneratedFloor> GeneratedFloorTemplate;

	UPROPERTY(EditAnywhere, Category = "SpawningValues")
	TSubclassOf<class AGeneratedObstacle> GeneratedObstacleTemplate;

	UPROPERTY(EditAnywhere, Category = "SpawningValues")
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, Category = "SpawningValues")
	class UInstancedStaticMeshComponent* ISMComp;

	UPROPERTY(EditAnywhere, Category = "SpawningValues")
	class UInstancedStaticMeshComponent* FullPiece;

	UPROPERTY(EditAnywhere, Category = "SpawningValues")
	class UInstancedStaticMeshComponent* SidePiece;

	UPROPERTY(EditAnywhere, Category = "SpawningValues")
	class UInstancedStaticMeshComponent* ConcaveCornerPiece;

	UPROPERTY(EditAnywhere, Category = "SpawningValues")
	class UInstancedStaticMeshComponent* ConvexCornerPiece;

	UPROPERTY(EditAnywhere, Category = "SpawningValues", meta = (ClampMin = "1", UIMin = "1", ClampMax = "100", UIMax = "100"))
	int SpawnWidth;

	UPROPERTY(EditAnywhere, Category = "SpawningValues", meta = (ClampMin = "1", UIMin = "1", ClampMax = "100", UIMax = "100"))
	int SpawnDepth;

	UPROPERTY(EditAnywhere, Category = "SpawningValues", meta = (ClampMin = "0", UIMin = "0", ClampMax = "100", UIMax = "100"))
	float FullPercentage;

	UPROPERTY(EditAnywhere, Category = "SpawningValues", meta = (ClampMin = "0", UIMin = "0", ClampMax = "100", UIMax = "100"))
	int CenterForceFull;

	UPROPERTY(EditAnywhere, Category = "SpawningValues", meta = (ClampMin = "0", UIMin = "0", ClampMax = "100", UIMax = "100"))
	int BorderForceFull;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	void GenerateFloor(int x, int y, TArray<FloorValues> &floorValues);

	void InitializeFloor(int x, int y, TArray<FloorValues>& floorValues);
	void ForceFloorBool(bool forcedFloor, int x, int y, TArray<FloorValues> &floorValues);
	void CheckFloor(int x, int y, TArray<FloorValues>& floorValues, TArray<FTransform>& fullFloorPieceArray, TArray<FTransform>& sideFloorPieceArray, TArray<FTransform>& concaveCornerPieceArray, TArray<FTransform>& convexCornerArray);

	void FloorCheckTopNeighbours(bool checkLeft, bool checkRight, int indexOffset, int x, int y, TArray<FloorValues>& floorValues);
	void FloorCheckMiddleNeighbours(bool checkLeft, bool checkRight, int indexOffset, int x, int y, TArray<FloorValues>& floorValues);
	void FloorCheckBottomNeighbours(bool checkLeft, bool checkRight, int indexOffset, int x, int y, TArray<FloorValues>& floorValues);

	void SetFloorShape(int x, int y, TArray<FloorValues>& floorValues, TArray<FTransform>& fullFloorPieceArray, TArray<FTransform>& sideFloorPieceArray, TArray<FTransform>& concaveCornerPieceArray, TArray<FTransform>& convexCornerArray);

	void SetTopLeftFloorShape(int x, int y, TArray<FloorValues>& floorValues, TArray<FTransform>& fullFloorPieceArray, TArray<FTransform>& sideFloorPieceArray, TArray<FTransform>& concaveCornerPieceArray, TArray<FTransform>& convexCornerArray);
	void SetTopRightFloorShape(int x, int y, TArray<FloorValues>& floorValues, TArray<FTransform>& fullFloorPieceArray, TArray<FTransform>& sideFloorPieceArray, TArray<FTransform>& concaveCornerPieceArray, TArray<FTransform>& convexCornerArray);
	void SetBottomLeftFloorShape(int x, int y, TArray<FloorValues>& floorValues, TArray<FTransform>& fullFloorPieceArray, TArray<FTransform>& sideFloorPieceArray, TArray<FTransform>& concaveCornerPieceArray, TArray<FTransform>& convexCornerArray);
	void SetBottomRightFloorShape(int x, int y, TArray<FloorValues>& floorValues, TArray<FTransform>& fullFloorPieceArray, TArray<FTransform>& sideFloorPieceArray, TArray<FTransform>& concaveCornerPieceArray, TArray<FTransform>& convexCornerArray);

	int GetMapWidth() {return SpawnWidth;}
	int GetMapDepth() {return SpawnDepth;}

	float GetFullPercentage() { return FullPercentage; }

	class AGeneratedFloor* GetGeneratedFloorAtIndex(int index) { return GeneratedFloorArray[index]; }

private:
	TArray<class AGeneratedFloor*> GeneratedFloorArray;
	TArray<class AGeneratedObstacle*> GeneratedObstacleArray;
};
