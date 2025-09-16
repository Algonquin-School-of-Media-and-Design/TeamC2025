#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawningCard.h"
#include "SpawnPoint.generated.h"

UCLASS()
class QUESTIONABLE_030_API ASpawnPoint : public AActor
{
	GENERATED_BODY()
protected:
	//The allowance to use for spawn on each wave
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave Properties", meta = (AllowPrivateAccess = "true"))
	float m_spawnAllowance;
	//The current allowance left to use for spawning
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave Properties", meta = (AllowPrivateAccess = "true"))
	float m_currentAllowance;
	//The number of waves of enemies this spawn point will spawn
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave Properties", meta = (AllowPrivateAccess = "true"))
	int m_numOfWaves;
	//The Current Wave
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave Properties", meta = (AllowPrivateAccess = "true"))
	int m_CurrentWaves;
	//The cards to use when deciding what to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Pool", meta = (AllowPrivateAccess = "true"))
	TArray<USpawnCard*> m_pSpawningCards;
	//The enemies currently in the scene
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy In Scene", meta = (AllowPrivateAccess = "true"))
	TArray<APawn*> m_pSpawnedEnemies;
	

	//I had to put this here because of a weird bug where it would not let me resign element into it after removing those same elements on a previous run of the spawn enemies otherwise it would be a local variable
	TArray<USpawnCard*> m_pPickableCards;

	//The maximum number of enemies that can spawn at the spawn point at the same time
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Point Properties", meta = (AllowPrivateAccess = "true", ClampMin = "1"))
	int m_maxNumOfEnemies;
	//The raduis at which enemies can spawn around the spawn point
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Point Properties", meta = (AllowPrivateAccess = "true", ClampMin = "10"))
	float m_radius;
	//How far away the enemies have to be in oder to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Point Properties", meta = (AllowPrivateAccess = "true"))
	float m_spawnSeparationDistance;
	//The height at which to start the raycast that looks for sold ground
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Point Properties", meta = (AllowPrivateAccess = "true", ClampMin = "10"))
	float m_heightToCheckFrom;
	//The depth at which to stop the raycast that looks for sold ground
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Point Properties", meta = (AllowPrivateAccess = "true", ClampMax = "-10"))
	float m_depthToCheckFrom;
	//Just to make it easier to move around
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* m_pMesh;
	//The Director
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director", meta = (AllowPrivateAccess = "true"))
	class ADirector* m_pDirector;

	
public:	
	// Sets default values for this actor's properties
	ASpawnPoint();
	//A method to check if the spawn point is maxed out on how many enemies it can spawn
	bool IsAtMaxCapacity();
	//Get a random position based on the spawn point properties to use for spawning
	FVector GetRandomPosition();

	//A method to try and spawn enemies on near by spawnPoints
	UFUNCTION()
	void SpawnEnemies();
	//A Function that will remove a provided enemy from m_pSpawnedEnemies. It is meant to be called only by APawn when a OnDestroy event happens
	UFUNCTION()
	void RemoveEnemy(AActor* enemy);
	//A method to check if the spawn point has finished all its waves
	UFUNCTION()
	bool FinishedAllWaves();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
