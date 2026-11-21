#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PuzzleManager.generated.h"

class APuzzlePiece;
class APuzzleSlot;
class ADoorBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPuzzleSolved);

// 배치 퍼즐 관리자 (컵 퍼즐, 약병 퍼즐 공용)
// - 같은 PuzzleID 를 가진 PuzzleSlot / PuzzlePiece 를 게임 시작 시 찾아서 묶는다
// - 모든 슬롯에 정답 조각이 놓이면 DoorToUnlock 의 잠금을 풀고 OnSolved 를 호출
UCLASS()
class GSD2_PROJECT_API APuzzleManager : public AActor
{
	GENERATED_BODY()

public:
	APuzzleManager();

	virtual void BeginPlay() override;

	// 조각 선택 (같은 조각을 다시 고르면 선택 해제)
	void SelectPiece(APuzzlePiece* Piece);

	// 선택된 조각을 슬롯에 놓는다 (슬롯에 다른 조각이 있으면 자리 교환)
	void PlaceSelectedOn(APuzzleSlot* Slot);

	// 퍼즐 구분용 이름 (예: Cup, Medicine). 같은 이름의 슬롯/조각만 이 관리자에 속한다
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	FName PuzzleID;

	// 풀렸을 때 잠금 해제할 문/서랍
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	TObjectPtr<ADoorBase> DoorToUnlock;

	// 풀렸을 때 문/서랍을 자동으로 열지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	bool bOpenDoorOnSolved = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	FString SolvedMessage = TEXT("딸깍. 어딘가 열리는 소리가 났다.");

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Puzzle")
	bool bSolved = false;

	UPROPERTY(BlueprintAssignable, Category = "Puzzle")
	FOnPuzzleSolved OnSolved;

private:
	void CheckSolved();

	UPROPERTY(VisibleAnywhere, Category = "Puzzle")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(Transient)
	TArray<TObjectPtr<APuzzleSlot>> Slots;

	UPROPERTY(Transient)
	TObjectPtr<APuzzlePiece> SelectedPiece;
};
