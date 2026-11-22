#include "InteractionComponent.h"
#include "InteractableBase.h"
#include "GSD2_ProjectPlayerController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractionComponent::TryInteract()
{
	if (AInteractableBase* Target = FindInteractable())
	{
		Target->Interact(GetOwner());
	}
}

AInteractableBase* UInteractionComponent::FindInteractable() const
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn) return nullptr;

	APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
	if (!PC || !PC->PlayerCameraManager) return nullptr;

	// 조사 화면 중에는 F 상호작용을 하지 않는다 (조사 화면은 마우스로 조작)
	if (const AGSD2_ProjectPlayerController* GamePC = Cast<AGSD2_ProjectPlayerController>(PC))
	{
		if (GamePC->IsInspecting()) return nullptr;
	}

	const FVector Start = PC->PlayerCameraManager->GetCameraLocation();
	const FVector End = Start + PC->PlayerCameraManager->GetCameraRotation().Vector() * InteractDistance;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Pawn);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		return Cast<AInteractableBase>(Hit.GetActor());
	}
	return nullptr;
}
