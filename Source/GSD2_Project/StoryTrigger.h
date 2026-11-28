#pragma once

#include "CoreMinimal.h"
#include "InteractableBase.h"
#include "StoryTrigger.generated.h"

class AStoryEvent;
class UBoxComponent;

// F 로 이야기 이벤트를 시작하는 조사 지점 (출입문, CCTV, 휴대폰 보관함 안 등)
// - RequiredFlag 가 있어야 나타나고, DoneFlag 가 생기면 사라진다
// - 사라진 동안에는 충돌을 꺼서 같은 자리의 다른 조사 지점이 조준되도록 한다
UCLASS()
class GSD2_PROJECT_API AStoryTrigger : public AInteractableBase
{
	GENERATED_BODY()

public:
	AStoryTrigger();

	virtual void Tick(float DeltaTime) override;
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetPromptText() const override;

	// 지금 F 로 조사할 수 있는지
	bool IsActive() const;

	// 조준 범위 (Visibility 만 막는 상자)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Story")
	TObjectPtr<UBoxComponent> Box;

	// 이 진행 표시가 있어야 나타남 (비우면 처음부터)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story")
	FName RequiredFlag;

	// 이 진행 표시가 생기면 사라짐 (보통 EventToPlay 의 FinishFlag)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story")
	FName DoneFlag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story")
	TObjectPtr<AStoryEvent> EventToPlay;

private:
	bool bUsed = false;
};
