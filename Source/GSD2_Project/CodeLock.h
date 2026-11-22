#pragma once

#include "CoreMinimal.h"
#include "InspectStation.h"
#include "CodeLock.generated.h"

class ADoorBase;
class UCodeLockWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCodeLockOpened);

// 숫자 자물쇠 (일기장 0830, 휴대폰 보관함 0211 공용)
// - F : 자물쇠를 가까이 보는 조사 화면(InspectCamera 시점)으로 전환 + 숫자 입력 UI(CodeLockWidget) 표시
// - UI 의 ▲/▼ 로 숫자를 바꾸고 [확인] 으로 열기 시도
// - 맞으면 DoorToUnlock 잠금 해제 + 진행 표시/목표/힌트 갱신, 틀리면 다시 입력
UCLASS()
class GSD2_PROJECT_API ACodeLock : public AInspectStation
{
	GENERATED_BODY()

public:
	ACodeLock();

	virtual void BeginPlay() override;
	virtual bool CanInspect() const override { return !bSolved; }
	virtual void OnInspectStart() override;
	virtual void OnInspectEnd() override;

	int32 GetNumDigits() const { return Digits.Num(); }
	int32 GetDigit(int32 Index) const { return Digits.IsValidIndex(Index) ? Digits[Index] : 0; }

	// Index 번째 자리 숫자를 Amount 만큼 올리거나 내린다 (0~9 순환)
	void ChangeDigit(int32 Index, int32 Amount);

	// 지금 숫자로 열기 시도
	void TryOpen();

	// 정답 (자릿수만큼 입력 칸이 생긴다)
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

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "CodeLock")
	bool bSolved = false;

	UPROPERTY(BlueprintAssignable, Category = "CodeLock")
	FOnCodeLockOpened OnOpened;

private:
	// 지금 입력된 숫자들 (자릿수만큼)
	TArray<int32> Digits;

	UPROPERTY(Transient)
	TObjectPtr<UCodeLockWidget> LockWidget;
};
