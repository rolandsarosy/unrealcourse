#include "Interactables/CItemChest.h"

#include "Net/UnrealNetwork.h"

ACItemChest::ACItemChest()
{
	bReplicates = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("BaseMesh");
	RootComponent = BaseMesh;

	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>("LidMesh");
	LidMesh->SetupAttachment(BaseMesh);

	bIsLocked = false;
	bIsLidOpened = false;
}

bool ACItemChest::AttemptUnlock_Implementation(AActor* InstigatorActor)
{
	// This implementation will be done by children of this class. This function mustn't be marked with the PURE_VIRTUAL macro as it uses Unreal's Reflection system and has the BlueprintNative flag.
	return true;
}

void ACItemChest::MulticastOnUnSuccessfulOpeningAttempt_Implementation()
{
	OnUnsuccessfullOpeningAttempt();
}

void ACItemChest::OnUnsuccessfullOpeningAttempt_Implementation()
{
	// This implementation will be done by children of this class. This function mustn't be marked with the PURE_VIRTUAL macro as it uses Unreal's Reflection system and has the BlueprintNative flag.
}

void ACItemChest::OnRep_IsLidOpened_Implementation()
{
	// This implementation will be done by children of this class. This function mustn't be marked with the PURE_VIRTUAL macro as it uses Unreal's Reflection system and has the BlueprintNative flag.
}

void ACItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	if (bIsLocked)
	{
		if (AttemptUnlock(InstigatorPawn))
		{
			bIsLocked = false;
			bIsLidOpened = true;
			OnRep_IsLidOpened();
		}
		else
		{
			MulticastOnUnSuccessfulOpeningAttempt();
		}
	}
	else
	{
		bIsLidOpened = !bIsLidOpened;
		OnRep_IsLidOpened();
	}
}

void ACItemChest::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACItemChest, bIsLidOpened)
}
