#include "Components/CKeycardComponent.h"

#include "Net/UnrealNetwork.h"
#include "unrealcourse/unrealcourse.h"

UCKeycardComponent::UCKeycardComponent()
{
	SetIsReplicatedByDefault(true);

#if !UE_BUILD_SHIPPING
	PrimaryComponentTick.bCanEverTick = false; // Enable this only for debugging and such.
#endif
}

void UCKeycardComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority() && !DefaultKeycardGameplayTags.IsEmpty()) // Server only
	{
		for (const FGameplayTag Tag : DefaultKeycardGameplayTags) { AddKeycardGameplayTag(Tag); }
	}
}

/**
 * @brief Called when the CurrentKeycardGameplayTags container is replicated.
 *
 * Compares the previous container with the current container to determine if any tags have been added or removed.
 *
 * @note Notifies observers locally.
 */
void UCKeycardComponent::OnRep_CurrentKeycardGameplayTags(FGameplayTagContainer PreviousKeycardGameplayTags)
{
	if (PreviousKeycardGameplayTags.Num() == CurrentKeycardGameplayTags.Num()) return;
	if (!Cast<APawn>(GetOwner())->IsLocallyControlled()) return;

	if (PreviousKeycardGameplayTags.Num() < CurrentKeycardGameplayTags.Num()) // Assume an Action got added.
	{
		for (const auto& Tag : CurrentKeycardGameplayTags)
		{
			if (!PreviousKeycardGameplayTags.HasTag(Tag)) { OnKeycardGameplayTagAdded.Broadcast(Tag); }
		}
	}

	if (PreviousKeycardGameplayTags.Num() > CurrentKeycardGameplayTags.Num()) // Assume an Action got removed.
	{
		for (const auto& Tag : PreviousKeycardGameplayTags)
		{
			if (!CurrentKeycardGameplayTags.HasTag(Tag)) { OnKeycardGameplayTagRemoved.Broadcast(Tag); }
		}
	}
	PreviousKeycardGameplayTags = CurrentKeycardGameplayTags;
}

void UCKeycardComponent::AddKeycardGameplayTag_Implementation(const FGameplayTag Tag)
{
	const FGameplayTagContainer TransientGameplayTagContainer = CurrentKeycardGameplayTags; // Create a separate tag for calling OnRep on server as OnRep does not get called automatically there.
	CurrentKeycardGameplayTags.AddTag(Tag);
	OnRep_CurrentKeycardGameplayTags(TransientGameplayTagContainer);
}

void UCKeycardComponent::RemoveKeycardGameplayTag_Implementation(const FGameplayTag Tag)
{
	const FGameplayTagContainer TransientGameplayTagContainer = CurrentKeycardGameplayTags; // Create a separate tag for calling OnRep on server as OnRep does not get called automatically there.
	if (CurrentKeycardGameplayTags.RemoveTag(Tag))
	{
		OnRep_CurrentKeycardGameplayTags(TransientGameplayTagContainer);
	}
}

void UCKeycardComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

#if !UE_BUILD_SHIPPING
	FString Msg;
	Msg.Append("Tags: ");

	for (const FGameplayTag Tag : CurrentKeycardGameplayTags)
	{
		Msg.Append(FString::Printf(TEXT("[%s], "), *Tag.GetTagName().ToString()));
	}

	if (Msg.Len() > 0)
	{
		Msg.RemoveAt(Msg.Len() - 2, 2);
	}

	LogOnScreen(this, Msg, FColor::Blue, 0.0F);
#endif
}

UCKeycardComponent* UCKeycardComponent::GetComponentFrom(AActor* FromActor) { return FromActor ? FromActor->FindComponentByClass<UCKeycardComponent>() : nullptr; }

void UCKeycardComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCKeycardComponent, CurrentKeycardGameplayTags);
}
