// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyZone.generated.h"

UCLASS()
class RELICRUNNERS_API AEnemyZone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyZone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void Server_HandleEnemyWavesCompleted();

public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default Values ")
	class USceneComponent* Origin;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default Values ")
	class UBoxComponent* TriggerBox;

	UPROPERTY(EditAnywhere, Category = "Default Values | Enemy Spawning Zone Actor");
	TSubclassOf<class ASpawnZone> SpawnZoneTemplate;

	UPROPERTY(Replicated)
	class ASpawnZone* SpawnZone;

	UPROPERTY(Replicated)
	bool IsActive;
};
