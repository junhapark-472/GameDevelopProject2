#pragma once

#include "CoreMinimal.h"
#include "InspectStation.h"
#include "CodeLock.generated.h"

class ACodeLock;
class ADoorBase;
class UStaticMeshComponent;
class UTextRenderComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCodeLockOpened);

// 숫자 다이얼 1칸 (CodeLock 이 게임 시작 시 생성, 직접 배치하지 않음)
// 잠금장치 화면에서 위 버튼(+) / 아래 버튼(-) 을 클릭해 0~9 를 바꾼다
UCLASS()
class GSD2_PROJECT_API ACodeDial : public AInteractableBase
{
	GENERATED_BODY()

public:
	ACodeDial();

	// 1인칭에서 F 를 누르면 자물쇠의 잠금장치 화면을 연다
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetPromptText() const override;

	// 숫자를 Amount 만큼 올리거나 내린다 (0~9 순환)
	void ChangeDigit(int32 Amount);
	int32 GetDigit() const { return Digit; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CodeLock")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CodeLock")
	TObjectPtr<UStaticMeshComponent> UpButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CodeLock")
	TObjectPtr<UStaticMeshComponent> DownButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CodeLock")
	TObjectPtr<UTextRenderComponent> DigitText;

	UPROPERTY(Transient)
	TObjectPtr<ACodeLock> Lock;

private:
	int32 Digit = 0;
};

// 숫자 자물쇠 (일기장 0830, 휴대폰 보관함 0211 공용)
// - F : 잠금장치 화면(InspectCamera 시점)으로 전환
// - 다이얼 위/아래 버튼 클릭으로 숫자 변경, 확인 버튼 클릭으로 열기 시도
// - 맞으면 DoorToUnlock 잠금 해제 + 진행 표시/목표/힌트 갱신, 틀리면 다시 입력
UCLASS()
class GSD2_PROJECT_API ACodeLock : public AInspectStation
{
	GENERATED_BODY()

public:
	ACodeLock();

	virtual void BeginPlay() override;
	virtual bool CanInspect() const override { return !bSolved; }
	virtual void HandleInspectClick(const FHitResult& Hit) override;

	// 지금 다이얼 숫자로 열기 시도
	void TryOpen();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CodeLock")
	TObjectPtr<UStaticMeshComponent> Plate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CodeLock")
	TObjectPtr<UStaticMeshComponent> ConfirmButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CodeLock")
	TObjectPtr<UTextRenderComponent> ConfirmText;

	// 정답 (자릿수만큼 다이얼이 생긴다)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CodeLock")
	FString Code = TEXT("0211");

	// 맞혔을 때 잠금 해제할 문/서랍/보관함
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CodeLock")
	TObjectPtr<ADoorBase> DoorToUnlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CodeLock")
	bool bOpenDoorOnSolved = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CodeLock")
	FString SolvedMessage = TEXT("딸깍. 잠금이 풀렸다.");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CodeLock")
	FString WrongMessage = TEXT("덜컥덜컥... 열리지 않는다.");

	// 풀렸을 때 GameInstance 에 남길 진행 표시 (예: DiaryOpened, PhoneBoxOpened)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CodeLock|Progress")
	FName SolvedFlag;

	// 풀린 뒤의 현재 목표 / 무전기 힌트 (비우면 그대로)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CodeLock|Progress")
	FText NextObjective;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CodeLock|Progress")
	FText NextHint;

	// 다이얼 사이 간격
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CodeLock")
	float DialSpacing = 14.f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "CodeLock")
	bool bSolved = false;

	UPROPERTY(BlueprintAssignable, Category = "CodeLock")
	FOnCodeLockOpened OnOpened;

private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<ACodeDial>> Dials;
};
