#pragma once

#include "CoreMinimal.h"
#include "Interactables/CBasePickup.h"
#include "CCoinReward.generated.h"

UCLASS(Abstract)
class UNREALCOURSE_API ACCoinReward : public ACBasePickup
{
	GENERATED_BODY()

public:
	ACCoinReward();

protected:
	virtual bool OnEffectTrigger(APawn* InstigatorPawn) override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Coins")
	int32 CoinsRewardAmount;
};
