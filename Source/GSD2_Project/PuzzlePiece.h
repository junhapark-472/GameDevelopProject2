#pragma once

#include "CoreMinimal.h"
#include "InteractableBase.h"
#include "PuzzlePiece.generated.h"

class APuzzleManager;
class APuzzleSlot;
class UStaticMeshComponent;
class UTextRenderComponent;

// 배치 퍼즐의 조각 (컵, 약병 등)
// F 로 고르면 살짝 들어 올려지고, 그 상태에서 PuzzleSlot 을 F 로 누르면 그 자리로 이동
UCLASS()
class GSD2_PROJECT_API APuzzlePiece : public AInteractableBase
{
	GENERATED_BODY()

public:
	APuzzlePiece();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetPromptText() const override;

	void SetSelected(bool bSelected);
	void MoveToSlot(APuzzleSlot* Slot);
	void MoveHome();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Puzzle")
	TObjectPtr<UStaticMeshComponent> Mesh;

	// 조각 이름 표시 (테스트용, bShowLabel 로 끄기)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Puzzle")
	TObjectPtr<UTextRenderComponent> Label;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	FName PuzzleID;

	// 조각 이름 (예: RedCup, B-02)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	FName PieceID;

	// 이름표에 보여줄 글씨 (예: B-02 Buspirone). 비우면 PieceID 를 보여줌
	// (TextRender 기본 글꼴은 한글이 안 나오므로 테스트용은 영문으로)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	bool bShowLabel = true;

	// 골랐을 때 들어 올리는 높이
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	float SelectLift = 15.f;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Puzzle")
	TObjectPtr<APuzzleSlot> CurrentSlot;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Puzzle")
	TObjectPtr<APuzzleManager> Manager;

private:
	void SetRestLocation(const FVector& NewLocation);

	FVector HomeLocation;
	FVector RestLocation;
};
