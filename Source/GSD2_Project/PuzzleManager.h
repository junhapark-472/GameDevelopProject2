#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PuzzleManager.generated.h"

class APuzzlePiece;
class APuzzleSlot;
class ADoorBase;
class UBillboardComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPuzzleSolved);

// 배치 퍼즐 관리자 (컵 퍼즐, 약병 퍼즐 공용)
// - 같은 PuzzleID 를 가진 PuzzleSlot / PuzzlePiece 를 게임 시작 시 자동으로 찾아 묶는다
// - 모든 슬롯에 정답 조각이 놓이면 DoorToUnlock 의 잠금을 풀고 OnSolved 를 호출
UCLASS()
class GSD2_PROJECT_API APuzzleManager : public AActor
{
	GENERATED_BODY()

public:
	APuzzleManager();

	virtual void BeginPlay() override;

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

	// [테스트 세트 만들기] 버튼이 사용할 정답 순서 (슬롯 1번부터)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle|Test")
	TArray<FName> TestSolution = { TEXT("B-02"), TEXT("D-04"), TEXT("A-01"), TEXT("C-03") };

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Puzzle")
	bool bSolved = false;

	UPROPERTY(BlueprintAssignable, Category = "Puzzle")
	FOnPuzzleSolved OnSolved;

	// 조각 선택 (같은 조각을 다시 고르면 선택 해제)
	void SelectPiece(APuzzlePiece* Piece);

	// 선택된 조각을 슬롯에 놓는다 (슬롯에 다른 조각이 있으면 자리 교환)
	void PlaceSelectedOn(APuzzleSlot* Slot);

	// 에디터 디테일 패널에 버튼으로 표시됨: 관리자 옆에 슬롯/조각을 TestSolution 개수만큼 생성
	UFUNCTION(CallInEditor, Category = "Puzzle|Test", meta = (DisplayName = "테스트 세트 만들기"))
	void SpawnTestSetup();

private:
	void CheckSolved();
	void ShowMessage(const FString& Message, FColor Color = FColor::White) const;

	UPROPERTY(VisibleAnywhere, Category = "Puzzle")
	TObjectPtr<USceneComponent> Root;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<UBillboardComponent> Sprite;
#endif

	UPROPERTY(Transient)
	TArray<TObjectPtr<APuzzleSlot>> Slots;

	UPROPERTY(Transient)
	TObjectPtr<APuzzlePiece> SelectedPiece;
};
