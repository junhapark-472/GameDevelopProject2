#pragma once

#include "CoreMinimal.h"
#include "InteractableBase.h"
#include "PuzzleSlot.generated.h"

class APuzzleManager;
class APuzzlePiece;
class UStaticMeshComponent;
class UTextRenderComponent;

// 배치 퍼즐의 자리 (식탁의 가족 자리, 약 보관함 칸 등)
// 조각을 고른 상태에서 이 자리를 F 로 누르면 조각이 여기로 옮겨진다
UCLASS()
class GSD2_PROJECT_API APuzzleSlot : public AInteractableBase
{
	GENERATED_BODY()

public:
	APuzzleSlot();

	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Interact_Implementation(AActor* Interactor) override;

	// 조각이 놓일 위치
	FVector GetPlaceLocation() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Puzzle")
	TObjectPtr<UStaticMeshComponent> Mesh;

	// 자리 번호 표시 (테스트용, bShowLabel 로 끄기)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Puzzle")
	TObjectPtr<UTextRenderComponent> Label;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	FName PuzzleID;

	// 표시용 이름 (예: 1, 2 / Father, Mother)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	FName SlotName;

	// 이 자리에 와야 하는 조각의 PieceID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	FName CorrectPieceID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	bool bShowLabel = true;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Puzzle")
	TObjectPtr<APuzzlePiece> CurrentPiece;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Puzzle")
	TObjectPtr<APuzzleManager> Manager;
};
