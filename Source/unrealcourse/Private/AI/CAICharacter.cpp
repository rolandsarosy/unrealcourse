#include "AI/CAICharacter.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "CWorldUserWidget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "Components/CAttributeComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"

ACAICharacter::ACAICharacter()
{
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComponent");
	AttributeComponent = CreateDefaultSubobject<UCAttributeComponent>("AttributeComponent");

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
}

void ACAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	PawnSensingComponent->OnSeePawn.AddDynamic(this, &ACAICharacter::OnSeePawn);
	AttributeComponent->OnHealthChanged.AddDynamic(this, &ACAICharacter::OnHealthChanged);
	AttributeComponent->OnDeath.AddDynamic(this, &ACAICharacter::OnDeath);
}

// ReSharper disable once CppMemberFunctionMayBeConst ~ Incorrect suggestion
void ACAICharacter::OnHealthChanged(AActor* InstigatorActor, UCAttributeComponent* UAttributeComponent, const float NewHealth, const float Delta)
{
	AddHealthBar();

	if (Delta < 0.0f)
	{
		if (InstigatorActor != this) SetTargetActor(Cast<APawn>(InstigatorActor), true);
		GetMesh()->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->TimeSeconds);
	}
}

void ACAICharacter::OnDeath(AActor* KillerActor, UCAttributeComponent* OwnerComponent)
{
	SetLifeSpan(3.5f);
	if (const AAIController* AIController = Cast<AAIController>(GetController())) AIController->BrainComponent->StopLogic("Got killed");
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();
	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetMesh()->SetAllBodiesSimulatePhysics(true);
}

/**
 * @brief Sets the target actor for the AI character.
 *
 * This method is used to set the target actor for the AI character, which is responsible for tracking and engaging the target.
 *
 * @param NewTarget     The new target actor to set.
 * @param ShouldOverrideCurrentTarget    Flag indicating whether the current target should be overridden even if it is alive.
 *
 */
void ACAICharacter::SetTargetActor(AActor* NewTarget, const bool ShouldOverrideCurrentTarget) const
{
	// I'm unsure how to get the Blackboard's values here in the Editor, since there is no clear BlackBoard in the context of the Character. TODO: Work this out properly in the future.
	const FName BlackboardKeyName = TEXT("TargetActor");

	AAIController* AIController = Cast<AAIController>(GetController());
	if (!ensure(AIController)) return;

	UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent();
	if (!ensure(BlackboardComponent)) return;

	AActor* CurrentTarget = Cast<AActor>(BlackboardComponent->GetValueAsObject(BlackboardKeyName));

	bool IsCurrentTargetAlive = false;
	if (CurrentTarget)
	{
		if (const UCAttributeComponent* CurrentTargetAttributeComponent = UCAttributeComponent::GetComponentFrom(CurrentTarget)) IsCurrentTargetAlive = CurrentTargetAttributeComponent->IsAlive();
	}

	if (ShouldOverrideCurrentTarget || !CurrentTarget || !IsCurrentTargetAlive) BlackboardComponent->SetValueAsObject(BlackboardKeyName, NewTarget);
}

// ReSharper disable once CppMemberFunctionMayBeConst - Incorrect suggestion 
void ACAICharacter::OnSeePawn(APawn* Pawn) { SetTargetActor(Pawn, false); }


void ACAICharacter::AddHealthBar()
{
	if (ActiveHealthBar == nullptr)
	{
		ActiveHealthBar = CreateWidget<UCWorldUserWidget>(GetWorld(), HealthBarWidgetClass);
		if (ActiveHealthBar)
		{
			ActiveHealthBar->AttachedActor = this;
			ActiveHealthBar->AddToViewport();
		}
	}
}
