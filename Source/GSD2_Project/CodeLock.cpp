#include "CodeLock.h"
#include "CodeLockWidget.h"
#include "DoorBase.h"
#include "GSD2FunctionLibrary.h"
#include "GSD2_ProjectPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ACodeLock::ACodeLock()
{
	// 기본 모양: 엔진 기본 큐브로 만든 작은 자물쇠 상자 (맵에 자물쇠 모델이 있으면 게임에서 숨겨서 사용)
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		DisplayMesh->SetStaticMesh(CubeMesh.Object);
	}
	DisplayMesh->SetRelativeScale3D(FVector(0.04f, 0.2f, 0.12f));

	// 조사 범위는 자물쇠 앞면, 조사 화면은 정면에서 확대
	InteractBox->SetBoxExtent(FVector(10.f, 14.f, 10.f));
	InteractBox->SetRelativeLocation(FVector(6.f, 0.f, 0.f));
	InspectCamera->SetRelativeLocation(FVector(60.f, 0.f, 0.f));
	InspectCamera->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));

	PromptText = FText::FromString(TEXT("잠금장치 조사하기"));
}

void ACodeLock::BeginPlay()
{
	Super::BeginPlay();

	// 정답 자릿수만큼 0 으로 시작
	Digits.Init(0, Code.Len());

	// 풀리기 전까지 숨김 (보이지도, 조준되지도 않게)
	for (AActor* Actor : RevealOnSolved)
	{
		if (Actor)
		{
			Actor->SetActorHiddenInGame(true);
			Actor->SetActorEnableCollision(false);
		}
	}
}

void ACodeLock::OnInspectStart()
{
	Super::OnInspectStart();

	// 숫자 입력 UI 를 만들어 화면에 띄운다 (08강 CreateWidget / AddToViewport)
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	LockWidget = CreateWidget<UCodeLockWidget>(PC, UCodeLockWidget::StaticClass());
	if (LockWidget)
	{
		LockWidget->Setup(this);
		LockWidget->AddToViewport();
	}
}

void ACodeLock::OnInspectEnd()
{
	Super::OnInspectEnd();

	if (LockWidget)
	{
		LockWidget->RemoveFromParent();
		LockWidget = nullptr;
	}
}

void ACodeLock::ChangeDigit(int32 Index, int32 Amount)
{
	if (bSolved || !Digits.IsValidIndex(Index)) return;

	Digits[Index] = ((Digits[Index] + Amount) % 10 + 10) % 10;
}

void ACodeLock::TryOpen()
{
	if (bSolved) return;

	FString Current;
	for (const int32 Digit : Digits)
	{
		Current.AppendInt(Digit);
	}

	if (Current != Code)
	{
		UGSD2FunctionLibrary::ShowMessage(WrongMessage, FLinearColor(1.f, 0.4f, 0.4f), 2.f);
		return;
	}

	bSolved = true;
	UGSD2FunctionLibrary::ShowMessage(SolvedMessage, FLinearColor::Green);

	if (DoorToUnlock)
	{
		DoorToUnlock->Unlock(bOpenDoorOnSolved);
	}

	UGSD2FunctionLibrary::SetProgressFlag(this, SolvedFlag);
	UGSD2FunctionLibrary::UpdateObjectiveAndHint(this, NextObjective, NextHint);

	for (AActor* Actor : RevealOnSolved)
	{
		if (Actor)
		{
			Actor->SetActorHiddenInGame(false);
			Actor->SetActorEnableCollision(true);
		}
	}

	OnOpened.Broadcast();

	// 풀렸으면 조사 화면을 닫고 1인칭으로 돌아간다
	if (AGSD2_ProjectPlayerController* PC = Cast<AGSD2_ProjectPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		PC->EndInspection();
	}
}
