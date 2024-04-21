#include "AI/CAICharacter.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CAttributeComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"

ACAICharacter::ACAICharacter()
{
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComponent");
	AttributeComponent = CreateDefaultSubobject<UCAttributeComponent>("AttributeComponent");

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
}

void ACAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	PawnSensingComponent->OnSeePawn.AddDynamic(this, &ACAICharacter::OnPawnSeen);
}

void ACAICharacter::OnPawnSeen(APawn* Pawn)
{
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		// I'm unsure how to get the Blackboard's values here in the Editor, since there is no clear BlackBoard in the context of the Character. 
		AIController->GetBlackboardComponent()->SetValueAsObject("TargetActor", Pawn);
	}
}
