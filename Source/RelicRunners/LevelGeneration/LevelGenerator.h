// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../RelicRunnersGameState.h"
#include "LevelGenerator.generated.h"

enum class EObjectiveType : uint8;

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
struct FSFloorValues
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

USTRUCT()
struct FSFloorObstacleByTexture
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	bool IsRandomYaw = false;

	UPROPERTY(EditAnywhere,meta = (EditCondition = "!IsRandomYaw",ClampMin = "0", UIMin = "0", ClampMax = "3", UIMax = "3"))
	int ObstacleYaw = 0;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class APackedLevelActor> ObstacleActor;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GenerationValues | Values", meta = (Bitmask, BitmaskEnum = EObjectiveType))
	int ObjectiveType;

	UPROPERTY(EditAnywhere, Category = "GenerationValues | Values", meta = (ClampMin = "1", UIMin = "1", ClampMax = "1000", UIMax = "1000"))
	float TileScale;

	UPROPERTY(EditAnywhere, Category = "GenerationValues | Values")
	bool GenerationIsRandom;

	UPROPERTY(EditAnywhere, Category = "GenerationValues | Values", meta = (EditCondition = "!GenerationIsRandom", ToolTip = "If not nothing, level generation will place tiles based on position and colour of each pixel in the texture."))
	class UTexture2D* LevelTexture;

	UPROPERTY(EditAnywhere, Category = "GenerationValues | Values", meta = (EditCondition = "!GenerationIsRandom", ToolTip = "If there are any specific modular pieces needed on a specific tile, define the colour and what spawns on top of every instance of that colour. Note: All non-white pixels in texture will have a floor set to that tile even if the specific colour isn't specified to have a modular piece."))
	TMap <FColor, FSFloorObstacleByTexture> TileColourToPackedActor;

	UPROPERTY(EditAnywhere, Category = "GenerationValues | Values", meta = (EditCondition = "GenerationIsRandom", ClampMin = "2", UIMin = "2", ClampMax = "100", UIMax = "100"))
	int SpawnWidth;

	UPROPERTY(EditAnywhere, Category = "GenerationValues | Values", meta = (EditCondition = "GenerationIsRandom", ClampMin = "2", UIMin = "2", ClampMax = "100", UIMax = "100"))
	int SpawnDepth;

	UPROPERTY(EditAnywhere, Category = "GenerationValues | Values", meta = (EditCondition = "GenerationIsRandom", ClampMin = "0", UIMin = "0", ClampMax = "100", UIMax = "100"))
	float FullPercentage;

	UPROPERTY(EditAnywhere, Category = "GenerationValues | Values", meta = (EditCondition = "GenerationIsRandom", ClampMin = "0", UIMin = "0", ClampMax = "100", UIMax = "100"))
	float BasicObstaclePercentage;

	UPROPERTY(EditAnywhere, Category = "GenerationValues | Values", meta = (EditCondition = "GenerationIsRandom", ClampMin = "0", UIMin = "0", ClampMax = "100", UIMax = "100"))
	int CenterForceFull;

	UPROPERTY(EditAnywhere, Category = "GenerationValues | Values", meta = (EditCondition = "GenerationIsRandom", ClampMin = "0", UIMin = "0", ClampMax = "100", UIMax = "100"))
	int BorderForceFull;

	UPROPERTY(EditAnywhere, Category = "GenerationValues | Values", meta = (EditCondition = "GenerationIsRandom", ClampMin = "0", UIMin = "0", ClampMax = "1000", UIMax = "1000"))
	int MaxKeyTileAmount;

	UPROPERTY(EditAnywhere, Category = "GenerationValues | Values", meta = (EditCondition = "GenerationIsRandom", ClampMin = "0", UIMin = "0", ClampMax = "1000", UIMax = "1000"))
	int MaxShopAmount;

	UPROPERTY(EditAnywhere, Category = "GenerationValues | Modular Obstacle");
	TArray <TSubclassOf<class APackedLevelActor>> PackedLevelArray;

	UPROPERTY(EditAnywhere, Category = "GenerationValues | Modular Obstacle");
	TSubclassOf<class APackedLevelActor> LevelStartPackedLevel;

	UPROPERTY(EditAnywhere, Category = "GenerationValues | Modular Obstacle");
	TSubclassOf<class APackedLevelActor> LevelEndPackedLevel;

	UPROPERTY(EditAnywhere, Category = "GenerationValues | Modular Obstacle| Key Tiles");
	TArray<TSubclassOf<class APackedLevelActor>> CapturableFlagPackedLevel;

	UPROPERTY(EditAnywhere, Category = "GenerationValues | Modular Floor")
	class UStaticMeshComponent* FullPiece;

	UPROPERTY(EditAnywhere, Category = "GenerationValues | Modular Floor")
	class UStaticMeshComponent* SidePiece;

	UPROPERTY(EditAnywhere, Category = "GenerationValues | Modular Floor")
	class UStaticMeshComponent* ConcaveCornerPiece;

	UPROPERTY(EditAnywhere, Category = "GenerationValues | Modular Floor")
	class UStaticMeshComponent* ConvexCornerPiece;

protected:

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

public:
	void GenerateFloor(int x, int y);

	void InitializeFloor();
	void ForceFloorBool(bool forcedFloor, int x, int y, int width);

	void InitializeModularObstacle(FSFloorValues& floorValue);

	void SetStartingAndEndingPoints(int startingIndex, int endingIndex);

	void SetKeyTile(int index);

	void FindStartToEndPath(int startingIndex, int targetIndex, int width);

	void CheckFloor(int x, int y, int width, int depth);

	void FloorCheckTopNeighbours(bool checkLeft, bool checkRight, int indexOffset, int x, int y, int width);
	void FloorCheckMiddleNeighbours(bool checkLeft, bool checkRight, int indexOffset, int x, int y, int width);
	void FloorCheckBottomNeighbours(bool checkLeft, bool checkRight, int indexOffset, int x, int y, int width);

	void SetFloorShape(int x, int y, int width);

	void SetTopLeftFloorShape(int x, int y, int width);
	void SetTopRightFloorShape(int x, int y, int width);
	void SetBottomLeftFloorShape(int x, int y, int width);
	void SetBottomRightFloorShape(int x, int y, int width);

	UFUNCTION(Server, Reliable)
	void Server_SpawnFloorObstacles(int x, int y, int width);

	void SpawnFloorObstacles(int x, int y, int width);

	UFUNCTION(Server, Reliable)
	void Server_SpawnFloorObstaclesByColour(int x, int y, int width, FColor colour);
	void SpawnFloorObstaclesByColour(int x, int y, int width, FColor colour);

	void CreateFloor();


	int GetMapWidth() { return SpawnWidth; }
	int GetMapDepth() { return SpawnDepth; }

	float GetFullPercentage() { return FullPercentage; }

public:
	UFUNCTION()
	void OnRep_FloorValuesArrayChange();

	UPROPERTY(ReplicatedUsing = OnRep_FloorValuesArrayChange)
	TArray<FSFloorValues> FloorValuesArray;

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
