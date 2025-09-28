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

UENUM()
enum class EHorizontalFloorShape : uint8
{
	Blank,
	Left,
	Right,
	Full,
};

UENUM()
enum class EVerticalFloorShape : uint8
{
	Blank,
	Up,
	Down,
	Full,
};

UCLASS()
class RELICRUNNERS_API AGeneratedFloor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGeneratedFloor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default Values")
	class USceneComponent* Origin;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UArrowComponent* DebugUpArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UArrowComponent* DebugDownArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UArrowComponent* DebugLeftArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UArrowComponent* DebugRightArrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (ClampMin = "0", UIMin = "0"))
	FVector Dimensions;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	EMainFloorState GetMainState() { return MainState; }

	void InitializeFloor(class ALevelGenerator* Owner, int xIndex, int yIndex);

	UFUNCTION()
	void SetMainState();

	UFUNCTION()
	void CheckNeighbours();

	void SetFloorShape();
	void CheckHorizontalNeighbour();
	void CheckVerticalNeighbour();

private:
	class ALevelGenerator* GeneratorOwner;

	EMainFloorState MainState;
	EHorizontalFloorShape HorizontalShape;
	EVerticalFloorShape VerticalShape;
	int XIndex;
	int YIndex;
};
