#pragma once

#include "CoreMinimal.h"
#include "InteractableBase.h"
#include "DoorBase.generated.h"

class AStoryEvent;
class USceneComponent;
class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDoorUnlocked);

// 잠금/열쇠 기능이 있는 문 (방문, 욕실 문, 서랍, 휴대폰 보관함 등에 재사용)
// - bLocked 가 true 이면 RequiredItemID 아이템이 인벤토리에 있어야 열림
// - 퍼즐이 풀렸을 때는 퍼즐 쪽에서 Unlock() 을 호출
UCLASS()
class GSD2_PROJECT_API ADoorBase : public AInteractableBase
{
	GENERATED_BODY()

public:
	ADoorBase();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetPromptText() const override;

	// 잠금 해제 (bAlsoOpen 이 true 면 바로 열기까지)
	UFUNCTION(BlueprintCallable, Category = "Door")
	void Unlock(bool bAlsoOpen = false);

	// 열려 있으면 닫고, 닫혀 있으면 연다
	UFUNCTION(BlueprintCallable, Category = "Door")
	void ToggleDoor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	TObjectPtr<USceneComponent> Root;

	// 문이 이 점을 중심으로 회전한다
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	TObjectPtr<USceneComponent> Hinge;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	TObjectPtr<UStaticMeshComponent> DoorMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	bool bLocked = false;

	// 이 아이템을 가지고 있으면 잠금이 풀린다 (비워두면 열쇠로는 못 엶)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	FName RequiredItemID;

	// 열쇠로 열었을 때 인벤토리에서 열쇠를 제거할지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	bool bConsumeKey = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float OpenAngle = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float OpenSpeed = 4.f;

	// true 면 회전하지 않고 SlideOffset 만큼 밀려 나온다 (서랍용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	bool bSlide = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door", meta = (EditCondition = "bSlide"))
	FVector SlideOffset = FVector(0.f, 30.f, 0.f);

	// true 면 한 번 열면 닫히지 않고, 열린 뒤에는 조준 대상에서 빠진다
	// (서랍 안 열쇠처럼 안에 있는 물건을 바로 집을 수 있도록)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	bool bStayOpen = false;

	// 열려 있을 때 표시할 문구
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	FText ClosePromptText;

	// 잠겨 있을 때 표시할 문구
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	FText LockedPromptText;

	// 함께 움직일 맵 액터의 이름 (예: 서랍 안 휴대폰). 게임이 시작되면 문/서랍에 붙인다
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	TArray<FName> AttachedActorNames;

	// 잠금이 풀렸을 때 바꿀 현재 목표 / 무전기 힌트 (비우면 그대로)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Progress")
	FText UnlockObjective;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Progress")
	FText UnlockHint;

	// 처음 열었을 때 재생할 이야기 이벤트 (예: 출입문 봉쇄 후 범인의 침입 시도)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Progress")
	TObjectPtr<AStoryEvent> OpenEvent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	FString LockedMessage = TEXT("잠겨 있다.");

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Door")
	bool bIsOpen = false;

	UPROPERTY(BlueprintAssignable, Category = "Door")
	FOnDoorUnlocked OnUnlocked;

private:
	// 0 = 닫힘, 1 = 열림
	float OpenAlpha = 0.f;

	bool bOpenEventPlayed = false;
};
