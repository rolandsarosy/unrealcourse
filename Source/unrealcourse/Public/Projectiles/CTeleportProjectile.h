#pragma once

#include "CoreMinimal.h"
#include "Projectiles/CBaseProjectile.h"
#include "CTeleportProjectile.generated.h"

class UParticleSystemComponent;

UCLASS()
class UNREALCOURSE_API ACTeleportProjectile : public ACBaseProjectile
{
	GENERATED_BODY()

public:
	ACTeleportProjectile();
	
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UParticleSystemComponent> TeleportExplosionEffectComponent;

private:
	// TODO: Fix this when it is confirmed that the class will not make significant changes to this anymore.
	// This is a bad approach in my opinion, but this is what the assignment specified. I'd have rather used a delegate listening to the end of one of the particle system's emitters.
	FTimerHandle TimerHandle_TeleportEffect;
	FTimerHandle TimerHandle_TeleportAction;

	virtual void PostInitializeComponents() override;

	void OnStartTeleportEffect();

	void OnTeleport();

	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
