#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableBase.generated.h"

UCLASS()
class GSD2_PROJECT_API AInteractableBase : public AActor
{
	GENERATED_BODY()

public:
	AInteractableBase();

	// 블루프린트에서 가져가서 오브젝트 상호작용 제작

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact(AActor* Interactor);
	virtual void Interact_Implementation(AActor* Interactor);
};