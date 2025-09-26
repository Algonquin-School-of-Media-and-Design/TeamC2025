#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawningCard.h"
#include "Director.generated.h"

class ARelicRunnersCharacter;

UCLASS()
class RELICRUNNERS_API ADirector : public AActor
{
	GENERATED_BODY()

protected:
	//The max distance away a spawn point can be to be used
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Stats", meta = (AllowPrivateAccess = "true"))
	float m_maxSpawnPointActivisionDistance;
	//The time it takes for the director to try and spawn again
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Stats", meta = (AllowPrivateAccess = "true"))
	float m_spawnWait;
	
	//Keeps track of how long it been sense the last spawn if it needs to spawn again
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timer", meta = (AllowPrivateAccess = "true"))
	float m_spawnTimer;

	//The man, The myth, the legend, the PLAYER 1!!!! and I guess all the side character if it multiplayer 
	TArray<APawn*> m_pPlayers;
	//If a spawn point does not have it spawn cards set it will use this
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Pool", meta = (AllowPrivateAccess = "true"))
	TArray<USpawnCard*> m_pDefaultSpawningCards;
	//The enemies currently in the scene
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy In Scene", meta = (AllowPrivateAccess = "true"))
	TArray<APawn*> m_pEnemiesInWorld;
	
public:	
	// Sets default values for this actor's properties
	ADirector();

	//A Method that will remove a provided enemy from m_pEnemiesInWorld. It is meant to be called only by APawn when a OnDestroy event happens
	UFUNCTION()
	void RemoveEnemy(AActor* enemy);
	//A Method that will remove a provided enemy from m_pEnemiesInWorld. It is meant to be called only by APawn when a OnDestroy event happens
	UFUNCTION()
	void AddEnemy(AActor* enemy);
	//A Method to get the default spawn cards
	UFUNCTION()
	TArray<USpawnCard*> GetDefaultSpawnCards();
	//A Method to get all the players that the director is keeping track of
	UFUNCTION()
	TArray<APawn*> GetPlayers();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//A method to try and spawn enemies on near by spawnPoints
	void SpawnCheck();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//virtual TSubclassOf<AAItemInWorld> GetRandomItem();

};
