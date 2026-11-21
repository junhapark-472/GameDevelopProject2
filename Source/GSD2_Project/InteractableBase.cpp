#include "InteractableBase.h"

AInteractableBase::AInteractableBase()
{
	PrimaryActorTick.bCanEverTick = false;

	PromptText = FText::FromString(TEXT("상호작용"));
}

void AInteractableBase::Interact_Implementation(AActor* Interactor)
{
}

FText AInteractableBase::GetPromptText() const
{
	return PromptText;
}
