#include "Components/CEnemySpawnerComponent.h"

#include "EngineUtils.h"
#include "AI/CAICharacter.h"
#include "Components/CAttributeComponent.h"
#include "EnvironmentQuery/EnvQueryManager.h"

static TAutoConsoleVariable CVarSpawnEnemies(TEXT("course.SpawnEnemies"), true, TEXT("Enabled or disables spawning of enemies via timers."), ECVF_Cheat);

UCEnemySpawnerComponent::UCEnemySpawnerComponent()
{
	EnemySpawnTimerInterval = 2.0f;
}

void UCEnemySpawnerComponent::BeginPlay()
{
	Super::BeginPlay();
	FTimerHandle TimerHandle_SpawnEnemies;
	GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle_SpawnEnemies, this, &UCEnemySpawnerComponent::OnSpawnEnemyTimerElapsed, EnemySpawnTimerInterval, true);
}

void UCEnemySpawnerComponent::OnSpawnEnemyTimerElapsed()
{
	if (!CVarSpawnEnemies.GetValueOnGameThread())
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy Spawn"))
		return;
	}

	if (CanGameModeSpawnMoreEnemies(GetNumberOfEnemiesAlive()))
	{
		UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(GetWorld(), SpawnEnemyEQ, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);

		if (!ensure(QueryInstance)) return;
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &UCEnemySpawnerComponent::OnQueryCompleted);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst ~ Incorrect suggestion
// ReSharper disable once CppParameterMayBeConstPtrOrRef ~ Incorrect suggestion
void UCEnemySpawnerComponent::OnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	switch (QueryStatus)
	{
	case EEnvQueryStatus::Success:
		if (QueryInstance->GetResultsAsLocations().IsValidIndex(0)) SpawnEnemyAtLocation(QueryInstance->GetResultsAsLocations()[0]);
	case EEnvQueryStatus::Processing:
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("Enemy spawn EQ did not succeed."));
	}
}

void UCEnemySpawnerComponent::SpawnEnemyAtLocation(const FVector& SpawnLocation) const
{
	const AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(SpawnEnemyClass, SpawnLocation, FRotator::ZeroRotator);

#if WITH_EDITOR
	UE_LOG(LogTemp, Log, TEXT("%s attempted to spawn enemy class of %s at %s. Return value was: %p"),
	       *GetNameSafe(this), *GetNameSafe(SpawnEnemyClass), *SpawnLocation.ToString(), *GetNameSafe(SpawnedActor));

	if (SpawnedActor)
	{
		DrawDebugSphere(GetWorld(), SpawnLocation, 15.0f, 16, FColor::Yellow, false, 5.0f, 0, 1);
		DrawDebugString(GetWorld(), SpawnLocation, TEXT("Enemy Spawned Here"), nullptr, FColor::Yellow, 5.0f, true);
	}
#endif
}

// TODO - This is a computationally expensive call. It was written during class, but I'd like to write a more efficient solution later on.
// ReSharper disable once CppTooWideScopeInitStatement ~ Results in worse readability
uint16 UCEnemySpawnerComponent::GetNumberOfEnemiesAlive() const
{
	uint16 NumberOfEnemiesAlive = 0;
	for (TActorIterator<ACAICharacter> Iterator(GetWorld()); Iterator; ++Iterator)
	{
		ACAICharacter* Enemy = *Iterator;

		const UCAttributeComponent* AttributeComponent = UCAttributeComponent::GetComponentFrom(Enemy);
		if (AttributeComponent && AttributeComponent->IsAlive()) NumberOfEnemiesAlive++;
	}

	return NumberOfEnemiesAlive;
}

bool UCEnemySpawnerComponent::CanGameModeSpawnMoreEnemies(uint16 NumberOfEnemiesAlive) const
{
	// Automatically reject enemy spawns if the difficulty curve hasn't been set yet.
	if (!ensureMsgf(MaxEnemyCountOverTimeCurve, TEXT("Difficulty curve asset must be set for GameMode."))) return false;

	const uint16 MaxNumberOfEnemiesAlive = MaxEnemyCountOverTimeCurve->GetFloatValue(GetWorld()->TimeSeconds);
	return NumberOfEnemiesAlive < MaxNumberOfEnemiesAlive;
}