#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CGameModeBase.generated.h"

class UCSaveGame;
class UCEnemySpawnerComponent;
class UCPickupSpawnerComponent;

UCLASS()
class UNREALCOURSE_API ACGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACGameModeBase();

	virtual void OnActorKilled(AActor* Victim, AActor* Killer);

protected:
	UFUNCTION(BlueprintCallable, Category="SaveGame")
	void WriteSaveGameToDisk() const;

	void LoadSaveGameFromDisk();

private:
	UPROPERTY(EditDefaultsOnly, Category="Components")
	TObjectPtr<UCPickupSpawnerComponent> PickupSpawnerComponent;

	UPROPERTY(EditDefaultsOnly, Category="Components")
	TObjectPtr<UCEnemySpawnerComponent> EnemySpawnerComponent;

	UPROPERTY(EditDefaultsOnly, Category="GameMode")
	float PlayerRespawnDelay;

	UPROPERTY()
	UCSaveGame* CurrentSaveGame;

	FString SaveGameSlotName;
	
	UFUNCTION()
	void RespawnPlayer(AController* PlayerController);

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
};