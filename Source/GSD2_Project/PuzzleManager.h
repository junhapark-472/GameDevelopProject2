#pragma once

#include "CoreMinimal.h"
#include "InspectStation.h"
#include "PuzzleManager.generated.h"

class APuzzlePiece;
class APuzzleSlot;
class ADoorBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPuzzleSolved);

// 배치 퍼즐 (컵 퍼즐, 약병 퍼즐 공용). 식탁/약병 보관함 위치에 배치한다
// - F : 퍼즐 화면(InspectCamera 시점)으로 전환
// - 마우스 클릭 : 조각 선택 → 자리 클릭으로 배치 (다른 조각이 있으면 서로 교체)
// - 같은 PuzzleID 를 가진 PuzzleSlot / PuzzlePiece 를 게임 시작 시 찾아서 묶는다
// - 모든 자리에 정답 조각이 놓이면 DoorToUnlock 잠금 해제 + 진행 표시/목표/힌트 갱신
UCLASS()
class GSD2_PROJECT_API APuzzleManager : public AInspectStation
{
	GENERATED_BODY()

public:
	APuzzleManager();

	virtual void BeginPlay() override;
	virtual bool CanInspect() const override { return !bSolved; }
	virtual void HandleInspectClick(const FHitResult& Hit) override;
	virtual void OnInspectEnd() override;

	// 조각 선택 (같은 조각을 다시 고르면 선택 해제)
	void SelectPiece(APuzzlePiece* Piece);

	// 선택된 조각을 자리에 놓는다 (자리에 다른 조각이 있으면 자리 교환)
	void PlaceSelectedOn(APuzzleSlot* Slot);

	// 퍼즐 구분용 이름 (예: Cup, Medicine). 같은 이름의 자리/조각만 이 퍼즐에 속한다
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	FName PuzzleID;

	// 풀렸을 때 잠금 해제할 문/서랍
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	TObjectPtr<ADoorBase> DoorToUnlock;

	// 풀렸을 때 문/서랍을 자동으로 열지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	bool bOpenDoorOnSolved = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	FString SolvedMessage = TEXT("어딘가 열린 것 같다.");

	// 풀렸을 때 GameInstance 에 남길 진행 표시 (예: CupPuzzleSolved)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle|Progress")
	FName SolvedFlag;

	// 풀린 뒤의 현재 목표 / 무전기 힌트 (비우면 그대로)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle|Progress")
	FText NextObjective;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle|Progress")
	FText NextHint;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Puzzle")
	bool bSolved = false;

	UPROPERTY(BlueprintAssignable, Category = "Puzzle")
	FOnPuzzleSolved OnSolved;

private:
	void CheckSolved();

	UPROPERTY(Transient)
	TArray<TObjectPtr<APuzzleSlot>> Slots;

	UPROPERTY(Transient)
	TObjectPtr<APuzzlePiece> SelectedPiece;
};
