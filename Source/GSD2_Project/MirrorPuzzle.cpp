#include "MirrorPuzzle.h"
#include "GSD2FunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

// ---------------- AFoggedMirror ----------------

AFoggedMirror::AFoggedMirror()
{
	// 기본 모양: 엔진 기본 큐브로 만든 폭 100, 높이 120, 두께 2 판 (앞면이 +X 방향)
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

	// 조사 범위는 거울 앞면, 조사 화면은 거울 정면에서 확대
	InteractBox->SetBoxExtent(FVector(10.f, 50.f, 60.f));
	InteractBox->SetRelativeLocation(FVector(10.f, 0.f, 0.f));
	InspectCamera->SetRelativeLocation(FVector(90.f, 0.f, 0.f));
	InspectCamera->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));

	PromptText = FText::FromString(TEXT("거울 조사하기"));
}

void AFoggedMirror::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	CodeText->SetText(FText::FromString(Code));
}

void AFoggedMirror::Reveal()
{
	if (bRevealed || GetWorldTimerManager().IsTimerActive(RevealTimer)) return;

	// 김이 서리는 시간만큼 기다렸다가 숫자 표시 (06강 타이머)
	GetWorldTimerManager().SetTimer(RevealTimer, this, &AFoggedMirror::FinishReveal, FMath::Max(RevealDelay, 0.01f), false);
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

	UGSD2FunctionLibrary::ShowMessage(RevealMessage, FLinearColor(0.7f, 0.85f, 1.f));
}

// ---------------- AFaucet ----------------

AFaucet::AFaucet()
{
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// 기본 모양: 엔진 기본 원기둥으로 만든 지름 30, 높이 50 (수도꼭지 대용). 바닥이 액터 위치에 오도록 올림
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		Mesh->SetStaticMesh(CylinderMesh.Object);
	}
	Mesh->SetRelativeScale3D(FVector(0.3f, 0.3f, 0.5f));
	Mesh->SetRelativeLocation(FVector(0.f, 0.f, 25.f));

	PromptText = FText::FromString(TEXT("수도꼭지 틀기"));
}

void AFaucet::Interact_Implementation(AActor* Interactor)
{
	// 배수구 조사 전에는 반응하지 않는다
	if (!UGSD2FunctionLibrary::HasProgressFlag(this, RequiredFlag)) return;

	if (bTurnedOn)
	{
		UGSD2FunctionLibrary::ShowMessage(AlreadyOnMessage);
		return;
	}

	bTurnedOn = true;
	UGSD2FunctionLibrary::ShowMessage(TurnOnMessage);

	if (Mirror)
	{
		Mirror->Reveal();
	}
}

FText AFaucet::GetPromptText() const
{
	// 조건을 만족하기 전에는 안내 문구를 띄우지 않는다
	return UGSD2FunctionLibrary::HasProgressFlag(this, RequiredFlag) ? PromptText : FText::GetEmpty();
}
