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

	void SetSelected(bool bSelected);
	void MoveToSlot(APuzzleSlot* Slot);
	void MoveHome();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Puzzle")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Puzzle")
	TObjectPtr<UTextRenderComponent> Label;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	FName PuzzleID;

	// 조각 이름 (예: RedCup, B-02)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	FName PieceID;

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
