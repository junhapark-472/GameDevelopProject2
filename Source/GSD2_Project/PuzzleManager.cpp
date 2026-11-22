#include "PuzzleManager.h"
#include "PuzzlePiece.h"
#include "PuzzleSlot.h"
#include "DoorBase.h"
#include "GSD2FunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

APuzzleManager::APuzzleManager()
{
	// 기본값: 식탁 크기 정도의 조사 범위, 위에서 내려다보는 퍼즐 화면
	InteractBox->SetBoxExtent(FVector(80.f, 120.f, 40.f));
	InspectCamera->SetRelativeLocation(FVector(-150.f, 0.f, 170.f));
	InspectCamera->SetRelativeRotation(FRotator(-50.f, 0.f, 0.f));

	PromptText = FText::FromString(TEXT("식탁 조사하기"));
}

void APuzzleManager::BeginPlay()
{
	Super::BeginPlay();

	// 레벨에 있는 자리/조각 중 PuzzleID 가 같은 것만 이 퍼즐에 연결
	TArray<AActor*> FoundActors;

	UGameplayStatics::GetAllActorsOfClass(this, APuzzleSlot::StaticClass(), FoundActors);
	for (AActor* Actor : FoundActors)
	{
		APuzzleSlot* Slot = Cast<APuzzleSlot>(Actor);
		if (Slot && Slot->PuzzleID == PuzzleID)
		{
			Slot->Manager = this;
			Slots.Add(Slot);
		}
	}

	UGameplayStatics::GetAllActorsOfClass(this, APuzzlePiece::StaticClass(), FoundActors);
	for (AActor* Actor : FoundActors)
	{
		APuzzlePiece* Piece = Cast<APuzzlePiece>(Actor);
		if (Piece && Piece->PuzzleID == PuzzleID)
		{
			Piece->Manager = this;
		}
	}
}

void APuzzleManager::HandleInspectClick(const FHitResult& Hit)
{
	if (bSolved) return;

	if (APuzzlePiece* Piece = Cast<APuzzlePiece>(Hit.GetActor()))
	{
		// 조각을 든 상태에서 자리에 놓인 다른 조각을 누르면 그 자리로 교체
		if (SelectedPiece && SelectedPiece != Piece && Piece->CurrentSlot)
		{
			PlaceSelectedOn(Piece->CurrentSlot);
		}
		else
		{
			SelectPiece(Piece);
		}
	}
	else if (APuzzleSlot* Slot = Cast<APuzzleSlot>(Hit.GetActor()))
	{
		PlaceSelectedOn(Slot);
	}
}

void APuzzleManager::OnInspectEnd()
{
	// 퍼즐 화면을 나가면 들고 있던 조각을 내려놓는다 (배치 상태는 그대로 유지)
	if (SelectedPiece)
	{
		SelectedPiece->SetSelected(false);
		SelectedPiece = nullptr;
	}

	Super::OnInspectEnd();
}

void APuzzleManager::SelectPiece(APuzzlePiece* Piece)
{
	if (bSolved || !Piece) return;

	if (SelectedPiece == Piece)
	{
		Piece->SetSelected(false);
		SelectedPiece = nullptr;
		return;
	}

	if (SelectedPiece)
	{
		SelectedPiece->SetSelected(false);
	}
	SelectedPiece = Piece;
	Piece->SetSelected(true);
}

void APuzzleManager::PlaceSelectedOn(APuzzleSlot* Slot)
{
	if (bSolved || !Slot) return;

	// 아무것도 안 들고 있으면: 자리 위 조각을 집는다
	if (!SelectedPiece)
	{
		if (Slot->CurrentPiece)
		{
			SelectPiece(Slot->CurrentPiece);
		}
		return;
	}

	APuzzlePiece* Piece = SelectedPiece;
	Piece->SetSelected(false);
	SelectedPiece = nullptr;

	if (Slot->CurrentPiece == Piece) return;

	APuzzleSlot* OldSlot = Piece->CurrentSlot;
	APuzzlePiece* Other = Slot->CurrentPiece;

	// 자리에 다른 조각이 있으면 자리를 바꾼다
	if (Other)
	{
		if (OldSlot)
		{
			Other->MoveToSlot(OldSlot);
		}
		else
		{
			Other->MoveHome();
		}
	}

	Piece->MoveToSlot(Slot);
	CheckSolved();
}

void APuzzleManager::CheckSolved()
{
	if (bSolved || Slots.Num() == 0) return;

	for (const APuzzleSlot* Slot : Slots)
	{
		if (!Slot->CurrentPiece || Slot->CurrentPiece->PieceID != Slot->CorrectPieceID)
		{
			return;
		}
	}

	bSolved = true;
	UGSD2FunctionLibrary::ShowMessage(SolvedMessage, FLinearColor::Green);

	if (DoorToUnlock)
	{
		DoorToUnlock->Unlock(bOpenDoorOnSolved);
	}

	UGSD2FunctionLibrary::SetProgressFlag(this, SolvedFlag);
	UGSD2FunctionLibrary::UpdateObjectiveAndHint(this, NextObjective, NextHint);

	OnSolved.Broadcast();
}
