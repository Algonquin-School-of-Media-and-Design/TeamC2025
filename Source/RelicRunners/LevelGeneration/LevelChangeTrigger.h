// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelChangeTrigger.generated.h"

UCLASS()
class RELICRUNNERS_API ALevelChangeTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelChangeTrigger();

	virtual void OnConstruction(const FTransform& transform) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnTriggerOverlap(UPrimitiveComponent* OverlapComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default Values")
	class USceneComponent* Origin;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default Values")
	class UBoxComponent* TriggerBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default Values")
	class UTextRenderComponent* LevelTargetTextRender;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Target Level")
	const TSoftObjectPtr<UWorld> TargetLevel;

	UFUNCTION()
	void ChangeLevel();

	UFUNCTION(NetMulticast, Reliable)
	void MC_ChangeLevel();

};
