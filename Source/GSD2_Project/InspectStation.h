#pragma once

#include "CoreMinimal.h"
#include "InteractableBase.h"
#include "InspectStation.generated.h"

class UBoxComponent;
class UCameraComponent;
class UStaticMeshComponent;

// 조사 시점 오브젝트 (컵/약병 퍼즐 화면, 숫자 자물쇠, 거울/배수구/메모 확대 등 공용 부모 클래스)
// - F : 이 오브젝트의 InspectCamera 로 시점 전환 + 마우스 커서 표시 (이동/시점 회전 막힘)
// - 마우스 왼쪽 클릭 : HandleInspectClick 호출 (자식 클래스에서 처리)
// - ESC 또는 마우스 오른쪽 : 원래 1인칭 시점으로 복귀
UCLASS()
class GSD2_PROJECT_API AInspectStation : public AInteractableBase
{
	GENERATED_BODY()

public:
	AInspectStation();

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetPromptText() const override;

	// 지금 조사 화면에 들어갈 수 있는지 (자식 클래스에서 조건 추가)
	virtual bool CanInspect() const { return true; }

	// 조사 화면에서 마우스 왼쪽 클릭 (클릭한 물체/컴포넌트 정보가 Hit 에 들어 있음)
	virtual void HandleInspectClick(const FHitResult& Hit) {}

	// 조사 화면 시작/종료 시 PlayerController 가 호출
	virtual void OnInspectStart();
	virtual void OnInspectEnd();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inspect")
	TObjectPtr<USceneComponent> Root;

	// 필요하면 모양을 넣는 메시 (비워 두면 안 보임)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inspect")
	TObjectPtr<UStaticMeshComponent> DisplayMesh;

	// F 로 조사할 수 있는 범위 (조사 화면 중에는 마우스 클릭을 막지 않도록 꺼짐)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inspect")
	TObjectPtr<UBoxComponent> InteractBox;

	// 조사 화면 카메라 (에디터 뷰포트에서 위치/각도 조절)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inspect")
	TObjectPtr<UCameraComponent> InspectCamera;

	// 처음 조사할 때 출력할 문구 (독백 등, 비우면 출력 안 함)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspect", meta = (MultiLine = true))
	FString FirstInspectMessage;

	// 처음 조사하면 GameInstance 에 남길 진행 표시 (예: DrainChecked, 비우면 안 남김)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspect")
	FName InspectFlag;

	// 시점 전환에 걸리는 시간(초)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspect")
	float BlendTime = 0.4f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Inspect")
	bool bInspecting = false;

protected:
	bool bInspectedOnce = false;
};
