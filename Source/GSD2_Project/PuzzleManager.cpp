#include "PuzzleManager.h"
#include "PuzzlePiece.h"
#include "PuzzleSlot.h"
#include "DoorBase.h"
#include "GSD2FunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

APuzzleManager::APuzzleManager()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
}

void APuzzleManager::BeginPlay()
{
	Super::BeginPlay();

	// 레벨에 있는 슬롯/조각 중 PuzzleID 가 같은 것만 이 관리자에 연결
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

	// 아무것도 안 들고 있으면: 슬롯 위 조각을 집거나 안내
	if (!SelectedPiece)
	{
		if (Slot->CurrentPiece)
		{
			SelectPiece(Slot->CurrentPiece);
		}
		else
		{
			UGSD2FunctionLibrary::ShowMessage(TEXT("먼저 놓을 물건을 골라야 한다."));
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

	OnSolved.Broadcast();
}
