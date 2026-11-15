#include "PuzzlePiece.h"
#include "PuzzleManager.h"
#include "PuzzleSlot.h"
#include "GSD2FunctionLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

APuzzlePiece::APuzzlePiece()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

	// 기본 모양: 엔진 기본 원기둥으로 만든 지름 30, 높이 50 (컵/약병 대용). 바닥이 액터 위치에 오도록 올림
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		Mesh->SetStaticMesh(CylinderMesh.Object);
	}
	Mesh->SetRelativeScale3D(FVector(0.3f, 0.3f, 0.5f));
	Mesh->SetRelativeLocation(FVector(0.f, 0.f, 25.f));

	Label = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Label"));
	Label->SetupAttachment(Root);
	Label->SetWorldSize(22.f);
	Label->SetHorizontalAlignment(EHTA_Center);
	Label->SetVerticalAlignment(EVRTA_TextBottom);
	Label->SetTextRenderColor(FColor(255, 220, 0));
	Label->SetRelativeLocation(FVector(0.f, 0.f, 58.f));
	Label->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PromptText = FText::FromString(TEXT("집기"));
}

void APuzzlePiece::BeginPlay()
{
	Super::BeginPlay();

	HomeLocation = GetActorLocation();
	RestLocation = HomeLocation;
}

void APuzzlePiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Label->IsVisible())
	{
		UGSD2FunctionLibrary::FaceCamera(this, Label);
	}
}

void APuzzlePiece::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	Label->SetText(FText::FromName(PieceID));
	Label->SetVisibility(bShowLabel && !PieceID.IsNone());
}

void APuzzlePiece::Interact_Implementation(AActor* Interactor)
{
	if (Manager)
	{
		Manager->SelectPiece(this);
	}
}

void APuzzlePiece::SetSelected(bool bSelected)
{
	SetActorLocation(RestLocation + FVector(0.f, 0.f, bSelected ? SelectLift : 0.f));
}

void APuzzlePiece::MoveToSlot(APuzzleSlot* Slot)
{
	if (!Slot) return;

	if (CurrentSlot && CurrentSlot->CurrentPiece == this)
	{
		CurrentSlot->CurrentPiece = nullptr;
	}

	CurrentSlot = Slot;
	Slot->CurrentPiece = this;
	SetRestLocation(Slot->GetPlaceLocation());
}

void APuzzlePiece::MoveHome()
{
	if (CurrentSlot && CurrentSlot->CurrentPiece == this)
	{
		CurrentSlot->CurrentPiece = nullptr;
	}

	CurrentSlot = nullptr;
	SetRestLocation(HomeLocation);
}

void APuzzlePiece::SetRestLocation(const FVector& NewLocation)
{
	RestLocation = NewLocation;
	SetActorLocation(RestLocation);
}
