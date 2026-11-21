#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

class AInteractableBase;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GSD2_PROJECT_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();

	// 플레이어 블루프린트의 F키 입력에서 호출
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void TryInteract();

	// 지금 바라보고 있는 상호작용 대상 (없으면 nullptr) - HUD 조준점 표시에도 사용
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	AInteractableBase* FindInteractable() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractDistance = 250.f;
};