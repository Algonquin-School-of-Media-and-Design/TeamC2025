// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelGenerator.generated.h"

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EFloorNeighbours : uint8
{
	Blank				= 0,
	TopLeft				= 1 << 0,		/*0000 0001*/
	TopMiddle			= 1 << 1,		/*0000 0010*/
	TopRight			= 1 << 2,		/*0000 0100*/
	MiddleLeft			= 1 << 3,		/*0000 1000*/
	MiddleRight			= 1 << 4,		/*0001 0000*/
	BottomLeft			= 1 << 5,		/*0010 0000*/
	BottomMiddle		= 1 << 6,		/*0100 0000*/
	BottomRight			= 1 << 7,		/*1000 0000*/
};
ENUM_CLASS_FLAGS(EFloorNeighbours);

UENUM(BlueprintType)
enum class EFloorObstacle : uint8
{
	None,
	Basic,
	KeyTile,
	Shop,
	Start,
	End,
};


USTRUCT()
struct FloorValues
{
	GENERATED_BODY()
public:
	UPROPERTY()
	bool IsFullTile = false;

	UPROPERTY()
	EFloorNeighbours FloorNeighbours = EFloorNeighbours::Blank;

	UPROPERTY()
	EFloorObstacle FloorObstacle = EFloorObstacle::None;

	UPROPERTY()
	int randomFloorToSpawn = 0;

	UPROPERTY()
	int obstacleYaw = 0;
};


UCLASS()
class RELICRUNNERS_API ALevelGenerator : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALevelGenerator();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USceneComponent* Origin;

	UPROPERTY(EditAnywhere, Category = "SpawningValues | Modular Obstacle");
	TArray <TSubclassOf<class APackedLevelActor>> PackedLevelArray;

	UPROPERTY(EditAnywhere, Category = "SpawningValues | Modular Obstacle");
	TSubclassOf<class APackedLevelActor> LevelStartPackedLevel;

	UPROPERTY(EditAnywhere, Category = "SpawningValues | Modular Obstacle");
	TSubclassOf<class APackedLevelActor> LevelEndPackedLevel;

	UPROPERTY(EditAnywhere, Category = "SpawningValues | Modular Floor")
	class UStaticMeshComponent* FullPiece;

	UPROPERTY(EditAnywhere, Category = "SpawningValues | Modular Floor")
	class UStaticMeshComponent* SidePiece;

	UPROPERTY(EditAnywhere, Category = "SpawningValues | Modular Floor")
	class UStaticMeshComponent* ConcaveCornerPiece;

	UPROPERTY(EditAnywhere, Category = "SpawningValues | Modular Floor")
	class UStaticMeshComponent* ConvexCornerPiece;

	UPROPERTY(EditAnywhere, Category = "SpawningValues | Values", meta = (ClampMin = "2", UIMin = "2", ClampMax = "100", UIMax = "100"))
	int SpawnWidth;

	UPROPERTY(EditAnywhere, Category = "SpawningValues | Values", meta = (ClampMin = "2", UIMin = "2", ClampMax = "100", UIMax = "100"))
	int SpawnDepth;

	UPROPERTY(EditAnywhere, Category = "SpawningValues | Values", meta = (ClampMin = "0", UIMin = "0", ClampMax = "100", UIMax = "100"))
	float FullPercentage;

	UPROPERTY(EditAnywhere, Category = "SpawningValues | Values", meta = (ClampMin = "0", UIMin = "0", ClampMax = "100", UIMax = "100"))
	float BasicObstaclePercentage;

	UPROPERTY(EditAnywhere, Category = "SpawningValues | Values", meta = (ClampMin = "0", UIMin = "0", ClampMax = "100", UIMax = "100"))
	int CenterForceFull;

	UPROPERTY(EditAnywhere, Category = "SpawningValues | Values", meta = (ClampMin = "0", UIMin = "0", ClampMax = "100", UIMax = "100"))
	int BorderForceFull;

	UPROPERTY(EditAnywhere, Category = "SpawningValues | Values", meta = (ClampMin = "1", UIMin = "1", ClampMax = "1000", UIMax = "1000"))
	float TileScale;

	UPROPERTY(EditAnywhere, Category = "SpawningValues | Values", meta = (ClampMin = "0", UIMin = "0", ClampMax = "1000", UIMax = "1000"))
	int MaxKeyTileAmount;

	UPROPERTY(EditAnywhere, Category = "SpawningValues | Values", meta = (ClampMin = "0", UIMin = "0", ClampMax = "1000", UIMax = "1000"))
	int MaxShopAmount;

protected:

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

public:
	void GenerateFloor(int x, int y);

	void InitializeFloor();
	void ForceFloorBool(bool forcedFloor, int x, int y);

	void InitializeModularObstacle(FloorValues& floorValue);

	void SetStartingAndEndingPoints(int startingIndex, int endingIndex);

	void SetKeyTile(int index);

	void FindStartToEndPath(int startingIndex, int targetIndex);

	void CheckFloor(int x, int y);

	void FloorCheckTopNeighbours(bool checkLeft, bool checkRight, int indexOffset, int x, int y);
	void FloorCheckMiddleNeighbours(bool checkLeft, bool checkRight, int indexOffset, int x, int y);
	void FloorCheckBottomNeighbours(bool checkLeft, bool checkRight, int indexOffset, int x, int y);

	void SetFloorShape(int x, int y);

	void SetTopLeftFloorShape(int x, int y);
	void SetTopRightFloorShape(int x, int y);
	void SetBottomLeftFloorShape(int x, int y);
	void SetBottomRightFloorShape(int x, int y);

	UFUNCTION(Server, Reliable)
	void Server_SpawnFloorObstacles(int x, int y);

	void SpawnFloorObstacles(int x, int y);

	void CreateFloor();


	int GetMapWidth() { return SpawnWidth; }
	int GetMapDepth() { return SpawnDepth; }

	float GetFullPercentage() { return FullPercentage; }

public:
	UFUNCTION()
	void OnRep_FloorValuesArrayChange();

	UPROPERTY(ReplicatedUsing = OnRep_FloorValuesArrayChange)
	TArray<FloorValues> FloorValuesArray;

	TArray<FTransform> FullFloorPieceTransforms;
	TArray<FTransform> SideFloorPieceTransforms;
	TArray<FTransform> ConcaveCornerPieceTransforms;
	TArray<FTransform> ConvexCornerTransforms;

	bool bHasSpawnedStartingPoint;
	bool bHasSpawnerEndingPoint;
	int CurrentlySpawnedBasicObstacles;
	int CurrentlySpawnedEnemyTowers;
	int CurrentlySpawnedShops;
};
