#include "PuzzleManager.h"
#include "PuzzlePiece.h"
#include "PuzzleSlot.h"
#include "DoorBase.h"
#include "EngineUtils.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Components/BillboardComponent.h"

APuzzleManager::APuzzleManager()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

#if WITH_EDITORONLY_DATA
	Sprite = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));
	if (Sprite)
	{
		Sprite->SetupAttachment(Root);
	}
#endif
}

void APuzzleManager::BeginPlay()
{
	Super::BeginPlay();

	Slots.Reset();
	for (TActorIterator<APuzzleSlot> It(GetWorld()); It; ++It)
	{
		if (It->PuzzleID == PuzzleID)
		{
			It->Manager = this;
			Slots.Add(*It);
		}
	}
	for (TActorIterator<APuzzlePiece> It(GetWorld()); It; ++It)
	{
		if (It->PuzzleID == PuzzleID)
		{
			It->Manager = this;
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
			ShowMessage(TEXT("먼저 놓을 물건을 골라야 한다."));
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
	ShowMessage(SolvedMessage, FColor::Green);

	if (DoorToUnlock)
	{
		DoorToUnlock->Unlock();
		if (bOpenDoorOnSolved && !DoorToUnlock->bIsOpen)
		{
			DoorToUnlock->ToggleDoor();
		}
	}

	OnSolved.Broadcast();
}

void APuzzleManager::ShowMessage(const FString& Message, FColor Color) const
{
	if (GEngine && !Message.IsEmpty())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.5f, Color, Message);
	}
}

void APuzzleManager::SpawnTestSetup()
{
	UWorld* World = GetWorld();
	if (!World) return;

	// 같은 PuzzleID 의 기존 슬롯/조각은 지우고 새로 만든다
	TArray<AActor*> ToDestroy;
	for (TActorIterator<APuzzleSlot> It(World); It; ++It)
	{
		if (It->PuzzleID == PuzzleID) ToDestroy.Add(*It);
	}
	for (TActorIterator<APuzzlePiece> It(World); It; ++It)
	{
		if (It->PuzzleID == PuzzleID) ToDestroy.Add(*It);
	}
	for (AActor* Actor : ToDestroy)
	{
		World->DestroyActor(Actor);
	}

	const int32 Count = TestSolution.Num();
	const FTransform Base = GetActorTransform();
	const float Spacing = 70.f;

	for (int32 i = 0; i < Count; ++i)
	{
		// 슬롯: 관리자 앞쪽으로 한 줄
		const FVector SlotLoc = Base.TransformPosition(FVector(0.f, (i - (Count - 1) * 0.5f) * Spacing, 0.f));
		APuzzleSlot* Slot = World->SpawnActorDeferred<APuzzleSlot>(APuzzleSlot::StaticClass(), FTransform(Base.GetRotation(), SlotLoc));
		if (Slot)
		{
			Slot->PuzzleID = PuzzleID;
			Slot->SlotName = FName(*FString::FromInt(i + 1));
			Slot->CorrectPieceID = TestSolution[i];
			Slot->FinishSpawning(FTransform(Base.GetRotation(), SlotLoc));
		}

		// 조각: 슬롯 줄 뒤쪽에, 정답과 다른 순서(거꾸로)로
		const FVector PieceLoc = Base.TransformPosition(FVector(-90.f, (i - (Count - 1) * 0.5f) * Spacing, 0.f));
		APuzzlePiece* Piece = World->SpawnActorDeferred<APuzzlePiece>(APuzzlePiece::StaticClass(), FTransform(Base.GetRotation(), PieceLoc));
		if (Piece)
		{
			Piece->PuzzleID = PuzzleID;
			Piece->PieceID = TestSolution[Count - 1 - i];
			Piece->FinishSpawning(FTransform(Base.GetRotation(), PieceLoc));
		}
	}

	MarkPackageDirty();
}
