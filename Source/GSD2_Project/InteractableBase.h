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

	virtual void BeginPlay() override;

	// 블루프린트에서 가져가서 오브젝트 상호작용 제작

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact(AActor* Interactor);
	virtual void Interact_Implementation(AActor* Interactor);

	// 조준했을 때 화면에 표시할 행동 이름 (예: 줍기, 문 열기) - 각 오브젝트에서 바꿔서 사용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText PromptText;

	// 화면에 표시할 문구 (상태에 따라 바꾸고 싶으면 자식 클래스에서 재정의)
	virtual FText GetPromptText() const;

	// 맵에 원래 놓여 있던 장식용 액터의 이름 (예: StaticMeshActor_599)
	// 게임이 시작되면 이 액터들을 숨기고, 이 오브젝트가 같은 모델로 대신 보여준다
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	TArray<FName> ReplacedActorNames;
};