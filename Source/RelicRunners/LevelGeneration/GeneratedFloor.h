// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GeneratedFloor.generated.h"

UENUM()
enum class EMainFloorState : uint8
{
	Blank,
	Full,
};

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EFloorNeighbours : uint8
{
	Blank = 0,
	TopLeft				= 1<<0,		/*0000 0001*/
	TopMiddle			= 1<<1,		/*0000 0010*/
	TopRight			= 1<<2,		/*0000 0100*/
	MiddleLeft			= 1<<3,		/*0000 1000*/
	MiddleRight			= 1<<4,		/*0001 0000*/
	BottomLeft			= 1<<5,		/*0010 0000*/
	BottomMiddle		= 1<<6,		/*0100 0000*/
	BottomRight			= 1<<7,		/*1000 0000*/
};
ENUM_CLASS_FLAGS(EFloorNeighbours);

UCLASS()
class RELICRUNNERS_API AGeneratedFloor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGeneratedFloor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default Values")
	class USceneComponent* Origin;

	UPROPERTY(EditAnywhere, Category = "SpawningValues")
	class UInstancedStaticMeshComponent* ISMComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UArrowComponent* DebugUpArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UArrowComponent* DebugDownArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UArrowComponent* DebugLeftArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UArrowComponent* DebugRightArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UArrowComponent* DebugUpLeftArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UArrowComponent* DebugUpRightArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UArrowComponent* DebugDownLeftArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UArrowComponent* DebugDownRightArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UTextRenderComponent* UpLeftCornerText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UTextRenderComponent* UpRightCornerText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UTextRenderComponent* DownLeftCornerText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UTextRenderComponent* DownRightCornerText;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (ClampMin = "0", UIMin = "0"))
	FVector Dimensions;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	EMainFloorState GetMainState() { return MainState; }

	void InitializeFloor(class ALevelGenerator* Owner, int xIndex, int yIndex);
	void ForceFloorState(bool forceState);

	UFUNCTION()
	void SetMainState();

	UFUNCTION()
	void CheckNeighbours();

	void CheckTopNeighbours(bool checkLeft, bool checkRight, int indexOffset);
	void CheckMiddleNeighbours(bool checkLeft, bool checkRight, int indexOffset);
	void CheckDownNeighbours(bool checkLeft, bool checkRight, int indexOffset);

	void SetFloorShape();

	void SetTopLeftShape();
	void SetTopRightShape();
	void SetBottomLeftShape();
	void SetBottomRightShape();

private:
	class ALevelGenerator* GeneratorOwner;

	EMainFloorState MainState;
	EFloorNeighbours FloorNeighbours;
	int XIndex;
	int YIndex;
};
