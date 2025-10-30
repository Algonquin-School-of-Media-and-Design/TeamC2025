#include "Director.h"
#include "SpawnPoint.h"
#include "RelicRunners/RelicRunnersCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ADirector::ADirector() 
{  
	m_maxSpawnPointActivisionDistance = 10000;
	m_spawnWait = 20;
	PrimaryActorTick.bCanEverTick = true;
	Tags.Add("Director");
	SetReplicates(true);
}

// Called when the game starts or when spawned
void ADirector::BeginPlay()
{
	Super::BeginPlay();
	if (GetLocalRole() == ROLE_Authority)
	{
		//sort enemies by their weights for the weight sums algorithm used when picking who to spawn
		m_pDefaultSpawningCards.Sort([](const USpawnCard& A, const USpawnCard& B)
			{
				return A.m_spawnWeight < B.m_spawnWeight;
			});

		//reseting some varialbes
		m_spawnTimer = 0;

		//setting the player pointer
		/*TArray<AActor*> m_pTempArray;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARelicRunnersCharacter::StaticClass(), m_pTempArray);
		for (int i = 0; i < m_pTempArray.Num(); i++)
		{
			m_pPlayers.Add(static_cast<APawn*>(m_pTempArray[i]));
		}*/
	}
	else if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		PrimaryActorTick.bCanEverTick = false;
	}
}

void ADirector::RemoveEnemy(AActor* enemy)
{
	//cast the actor point as a APawn then check to make sure it when through before destroying it
	APawn* enemyPtr = static_cast<APawn*>(enemy);

	if (enemyPtr)
	{
		m_pEnemiesInWorld.Remove(enemyPtr);
	}
}

void ADirector::AddEnemy(AActor* enemy)
{
	//cast the actor point as a APawn then check to make sure it when through before adding it
	APawn* enemyPtr = static_cast<APawn*>(enemy);

	if (enemyPtr)
	{
		m_pEnemiesInWorld.Add(enemyPtr);
	}
}

TArray<USpawnCard*> ADirector::GetDefaultSpawnCards()
{
	return m_pDefaultSpawningCards;
}

TArray<APawn*> ADirector::GetPlayers()
{
	return m_pPlayers;
}

void ADirector::AddPlayer(APawn* newPlayer)
{
	if (!m_pPlayers.Contains(newPlayer))
	{
		m_pPlayers.Add(newPlayer);
	}
}

void ADirector::SpawnCheck()
{
	//Setting up the array
	TArray<AActor*> spawnPoints;

	//grab all spawnpoints in the scene
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnPoint::StaticClass(), spawnPoints);

	for (int i = 0; i < spawnPoints.Num(); i++)
	{
		ASpawnPoint* point = static_cast<ASpawnPoint*>(spawnPoints[i]);

		for (APawn* player : m_pPlayers)
		{
			if (!player) continue;

			//check to see if the player is close enough and the spawn point is not at max capacity
			if (FVector::Dist2D(player->GetActorLocation(), point->GetActorLocation()) < m_maxSpawnPointActivisionDistance && !point->IsAtMaxCapacity())
			{
				point->SpawnEnemies();
			}
		}
	}
}

// Called every frame
void ADirector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetLocalRole() == ROLE_Authority)
	{
		//check to see if it is time for a spawn check
		if (m_spawnTimer <= 0)
		{
			SpawnCheck();
			m_spawnTimer = m_spawnWait;
		}
		else
		{
			m_spawnTimer -= DeltaTime;

			if (m_spawnTimer < 0)
			{
				m_spawnTimer = 0;
			}
		}
	}
}

