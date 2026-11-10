#include "InteractionComponent.h"
#include "InteractableBase.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractionComponent::TryInteract()
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn) return;

	APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
	if (!PC || !PC->PlayerCameraManager) return;

	const FVector Start = PC->PlayerCameraManager->GetCameraLocation();
	const FVector End = Start + PC->PlayerCameraManager->GetCameraRotation().Vector() * InteractDistance;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Pawn);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		if (AInteractableBase* Target = Cast<AInteractableBase>(Hit.GetActor()))
		{
			Target->Interact(Pawn);
		}
	}
}