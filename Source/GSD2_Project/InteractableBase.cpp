#include "InteractableBase.h"

AInteractableBase::AInteractableBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AInteractableBase::Interact_Implementation(AActor* Interactor)
{
}
