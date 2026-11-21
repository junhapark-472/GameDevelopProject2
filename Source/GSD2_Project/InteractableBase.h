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

	// 조준했을 때 화면에 표시할 행동 이름 (예: 줍기, 문 열기) - 각 오브젝트에서 바꿔서 사용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText PromptText;

	// 화면에 표시할 문구 (상태에 따라 바꾸고 싶으면 자식 클래스에서 재정의)
	virtual FText GetPromptText() const;
};