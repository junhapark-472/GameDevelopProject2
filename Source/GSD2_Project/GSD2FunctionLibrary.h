#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GSD2FunctionLibrary.generated.h"

class USceneComponent;

// 여러 오브젝트에서 같이 쓰는 함수 모음 (07강 블루프린트 함수 라이브러리)
UCLASS()
class GSD2_PROJECT_API UGSD2FunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// 화면 왼쪽 위에 안내 문구 출력 (대사창 UI가 생기기 전까지 임시로 사용)
	UFUNCTION(BlueprintCallable, Category = "GSD2")
	static void ShowMessage(const FString& Message, FLinearColor Color = FLinearColor(1.f, 1.f, 1.f), float Duration = 3.f);

	// GameInstance 에 진행 표시(Flag) 남기기 (비어 있으면 아무것도 안 함)
	UFUNCTION(BlueprintCallable, Category = "GSD2", meta = (WorldContext = "WorldContextObject"))
	static void SetProgressFlag(const UObject* WorldContextObject, FName Flag);

	// 진행 표시가 있는지 (Flag 가 비어 있으면 조건 없음으로 보고 true)
	UFUNCTION(BlueprintPure, Category = "GSD2", meta = (WorldContext = "WorldContextObject"))
	static bool HasProgressFlag(const UObject* WorldContextObject, FName Flag);

	// 현재 목표 / 무전기 힌트 바꾸기 (비어 있는 쪽은 그대로 둠)
	UFUNCTION(BlueprintCallable, Category = "GSD2", meta = (WorldContext = "WorldContextObject"))
	static void UpdateObjectiveAndHint(const UObject* WorldContextObject, const FText& Objective, const FText& Hint);

	// 컴포넌트(글씨 등)가 플레이어 카메라 쪽을 바라보도록 좌우 회전
	UFUNCTION(BlueprintCallable, Category = "GSD2", meta = (WorldContext = "WorldContextObject"))
	static void FaceCamera(const UObject* WorldContextObject, USceneComponent* Component);
};
