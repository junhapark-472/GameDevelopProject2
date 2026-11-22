#include "InteractableBase.h"
#include "Kismet/GameplayStatics.h"

AInteractableBase::AInteractableBase()
{
	PrimaryActorTick.bCanEverTick = false;

	PromptText = FText::FromString(TEXT("상호작용"));
}

void AInteractableBase::BeginPlay()
{
	Super::BeginPlay();

	if (ReplacedActorNames.Num() == 0) return;

	// 이름이 같은 액터를 찾아서 숨기고 충돌도 끈다 (GetAllActorsOfClass)
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(this, AActor::StaticClass(), FoundActors);
	for (AActor* Actor : FoundActors)
	{
		if (Actor && ReplacedActorNames.Contains(Actor->GetFName()))
		{
			Actor->SetActorHiddenInGame(true);
			Actor->SetActorEnableCollision(false);
		}
	}
}

void AInteractableBase::Interact_Implementation(AActor* Interactor)
{
}

FText AInteractableBase::GetPromptText() const
{
	return PromptText;
}
