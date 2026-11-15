#include "MirrorPuzzle.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

static void ShowPuzzleMessage(const FString& Message, FColor Color = FColor::White)
{
	if (GEngine && !Message.IsEmpty())
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, Color, Message);
	}
}

// ---------------- AFoggedMirror ----------------

AFoggedMirror::AFoggedMirror()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// 기본 모양: 폭 100, 높이 120, 두께 2 인 판 (앞면이 +X 방향)
	MirrorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MirrorMesh"));
	MirrorMesh->SetupAttachment(Root);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		MirrorMesh->SetStaticMesh(CubeMesh.Object);
	}
	MirrorMesh->SetRelativeScale3D(FVector(0.02f, 1.f, 1.2f));

	CodeText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("CodeText"));
	CodeText->SetupAttachment(Root);
	CodeText->SetRelativeLocation(FVector(2.f, 0.f, 0.f));
	CodeText->SetHorizontalAlignment(EHTA_Center);
	CodeText->SetVerticalAlignment(EVRTA_TextCenter);
	CodeText->SetWorldSize(40.f);
	CodeText->SetTextRenderColor(FColor(230, 240, 255));
	CodeText->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CodeText->SetHiddenInGame(true);
}

void AFoggedMirror::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (CodeText)
	{
		CodeText->SetText(FText::FromString(Code));
	}
}

void AFoggedMirror::Reveal()
{
	if (bRevealed || GetWorldTimerManager().IsTimerActive(RevealTimer)) return;

	if (RevealDelay > 0.f)
	{
		GetWorldTimerManager().SetTimer(RevealTimer, this, &AFoggedMirror::FinishReveal, RevealDelay, false);
	}
	else
	{
		FinishReveal();
	}
}

void AFoggedMirror::FinishReveal()
{
	bRevealed = true;

	if (FoggedMaterial)
	{
		MirrorMesh->SetMaterial(0, FoggedMaterial);
	}
	CodeText->SetText(FText::FromString(Code));
	CodeText->SetHiddenInGame(false);

	ShowPuzzleMessage(RevealMessage, FColor(180, 220, 255));
}

// ---------------- AFaucet ----------------

AFaucet::AFaucet()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	// 기본 모양: 지름 10, 높이 20 인 원기둥
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		Mesh->SetStaticMesh(CylinderMesh.Object);
	}
	Mesh->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.2f));
}

void AFaucet::Interact_Implementation(AActor* Interactor)
{
	if (bTurnedOn)
	{
		ShowPuzzleMessage(AlreadyOnMessage);
		return;
	}

	bTurnedOn = true;
	ShowPuzzleMessage(TurnOnMessage);

	if (Mirror)
	{
		Mirror->Reveal();
	}
}
