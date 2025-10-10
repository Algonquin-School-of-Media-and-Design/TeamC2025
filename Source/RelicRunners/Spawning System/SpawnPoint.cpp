#include "SpawnPoint.h"
#include <Kismet/GameplayStatics.h>
#include "Director.h"

// Sets default values
ASpawnPoint::ASpawnPoint()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		m_pMesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
		SetRootComponent(m_pMesh);
		m_maxNumOfEnemies = 999;
		m_radius = 100;
		m_heightToCheckFrom = 200;
		m_depthToCheckFrom = 100;
		PrimaryActorTick.bCanEverTick = false;
	}

}

bool ASpawnPoint::IsAtMaxCapacity()
{
	return m_pSpawnedEnemies.Num() >= m_maxNumOfEnemies;
}

FVector ASpawnPoint::GetRandomPosition()
{
	bool hasFoundHitPoint = false;
	FHitResult hit(ForceInit);
	int numberOfTries = 0;
	FRandomStream rand;
	rand.GenerateNewSeed();

	//loop until we find a suitable location or we have tried 10 times as their could be no suitable spots
	do
	{
		numberOfTries++;
		hasFoundHitPoint = true;

		//getting said random vector and making both 2d and a direction vector
		FVector vec = rand.GetUnitVector();
		vec.Z = 0;
		vec.Normalize();

		//adjusting it for the radius and adding the spawn points location
		vec *= rand.FRandRange(0, m_radius);
		vec += GetActorLocation();

		//Setting up the collision query parameters for the raycast
		FCollisionQueryParams traceParams;
		traceParams.AddIgnoredActor(this);
		traceParams.bTraceComplex = true;

		//Do the raycast
		GetWorld()->LineTraceSingleByChannel(hit, vec + FVector(0, 0, m_heightToCheckFrom), vec - FVector(0, 0, m_depthToCheckFrom), ECC_PhysicsBody, traceParams);

		//DrawDebugLine(GetWorld(), vec + FVector(0, 0, m_heightToCheckFrom), vec - FVector(0, 0, m_depthToCheckToSpawnFrom), FColor::Red, false, 1, 0, 1.f);

		//check to make sure their is enough space between the players and all the enemies
		for (APawn* player : m_pDirector->GetPlayers())
		{
			if (FVector::Dist2D(player->GetActorLocation(), hit.ImpactPoint) <= m_spawnSeparationDistance)
			{
				hasFoundHitPoint = false;
				break;
			}
		}

		if (numberOfTries == 10 && !hasFoundHitPoint)
		{
			UE_LOG(LogTemp, Warning, TEXT("Spawn Point could not find a suitable location to spawn after 10 tries"));
			return FVector::ZeroVector;
		}

	} while (!hasFoundHitPoint);

	//return the new point
	return hit.ImpactPoint;
}

void ASpawnPoint::SpawnEnemies()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if ((m_pSpawnedEnemies.Num() == 0 && !FinishedAllWaves()) || m_currentAllowance > 0)
		{
			//if we have used up all our allowance for this wave then reset it and move to the next wave
			if (m_currentAllowance <= 0)
			{
				m_currentAllowance = m_spawnAllowance;
				m_CurrentWaves++;
			}

			//m_pPickableCards copies m_pSpawningCards or m_pDefaultSpawningCards that way we can remove cards from the pool we have to pick from without losing them on the next call
			if (m_pSpawningCards.Num() > 0)
			{
				m_pPickableCards = m_pSpawningCards;
			}
			else
			{
				m_pPickableCards = m_pDirector->GetDefaultSpawnCards();
			}

			//loop through until their is no more allowance for spawning enemies
			while (m_currentAllowance > 0)
			{
				if (IsAtMaxCapacity())
				{
					return;
				}

				//set up local variables
				int pickedEnemyIndex = -1;
				bool pickedSuccessful = false;

				//loop until we have pick a enemy to spawn
				while (!pickedSuccessful)
				{
					//If their are not more cards to pick from then leave the method because we can't spawn any more enemies
					if (m_pPickableCards.Num() <= 0)
					{
						m_currentAllowance = 0;
						return;
					}

					float sumOfWeights = 0;

					//add up all the weights of the enemies to find the max weight
					for (int i = 0; i < m_pPickableCards.Num(); i++)
					{
						sumOfWeights += m_pPickableCards[i]->m_spawnWeight;
					}

					//pick a random weight 
					float randomValue = FMath::FRandRange(0, sumOfWeights);

					//loop through and check the weight if the random is less then the weight that our enemy if not subtract the weight from random
					for (int i = 0; i < m_pPickableCards.Num(); i++)
					{
						if (randomValue < m_pPickableCards[i]->m_spawnWeight)
						{
							//check to make sure it in the budget for spawning
							if (m_currentAllowance >= m_pPickableCards[i]->m_spawnCost)
							{
								pickedEnemyIndex = i;
								m_currentAllowance -= m_pPickableCards[i]->m_spawnCost;
								pickedSuccessful = true;
								break;
							}
							//if not remove it from the pool
							else
							{
								m_pPickableCards.RemoveAt(i);
								break;
							}

						}

						randomValue -= m_pPickableCards[i]->m_spawnWeight;
					}
				}

				if (m_currentAllowance < 0)
				{
					m_currentAllowance = 0;
				}

				for (int i = 0; i < 10; i++)
				{
					APawn* newEnemy = static_cast<APawn*>(GetWorld()->SpawnActor<APawn>(m_pPickableCards[pickedEnemyIndex]->m_enemyType, GetRandomPosition() + FVector(0, 0, 400), FRotator::ZeroRotator));

					//spawning the enemy and adding it to the array
					if (newEnemy)
					{
						newEnemy->OnDestroyed.AddDynamic(this, &ASpawnPoint::RemoveEnemy);
						m_pSpawnedEnemies.Add(newEnemy);
						break;
					}
					else
					{
						//m_currentAllowance += m_pPickableCards[pickedEnemyIndex]->m_spawnCost;
						continue;
					}
				}
			}
		}
	}
}

void ASpawnPoint::RemoveEnemy(AActor* enemy)
{
	//cast the actor point as a enemy then check to make sure it when through before destroying it
	APawn* enemyPtr = static_cast<APawn*>(enemy);

	if (enemyPtr)
	{
		m_pSpawnedEnemies.Remove(enemyPtr);
	}
}

bool ASpawnPoint::FinishedAllWaves()
{
	return m_CurrentWaves >= m_numOfWaves;
}

// Called when the game starts or when spawned
void ASpawnPoint::BeginPlay()
{
	Super::BeginPlay();	
	if (GetLocalRole() == ROLE_Authority)
	{
		m_CurrentWaves = 0;

		if (!m_pDirector)
		{
			m_pDirector = Cast<ADirector>(UGameplayStatics::GetActorOfClass(GetWorld(), ADirector::StaticClass()));

			if (!m_pDirector)
			{
				UE_LOG(LogTemp, Error, TEXT("No ADirector found in scene"));
			}
		}

		//sort enemies by their weights for the weight sums algorithm used when picking who to spawn
		m_pSpawningCards.Sort([](const USpawnCard& A, const USpawnCard& B)
			{
				return A.m_spawnWeight < B.m_spawnWeight;
			});
	}
}