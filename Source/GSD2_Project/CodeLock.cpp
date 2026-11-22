#include "CodeLock.h"
#include "DoorBase.h"
#include "GSD2FunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"

// ---------------- ACodeDial ----------------

ACodeDial::ACodeDial()
{
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));

	// 숫자 칸: 엔진 기본 큐브로 만든 폭 12, 높이 18, 두께 6 블록 (앞면이 +X)
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
	Mesh->SetRelativeScale3D(FVector(0.06f, 0.12f, 0.18f));

	// 위/아래 버튼: 숫자 칸 위아래의 작은 블록
	UpButton = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UpButton"));
	UpButton->SetupAttachment(Root);
	UpButton->SetRelativeLocation(FVector(0.f, 0.f, 14.f));
	UpButton->SetRelativeScale3D(FVector(0.05f, 0.1f, 0.06f));

	DownButton = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DownButton"));
	DownButton->SetupAttachment(Root);
	DownButton->SetRelativeLocation(FVector(0.f, 0.f, -14.f));
	DownButton->SetRelativeScale3D(FVector(0.05f, 0.1f, 0.06f));

	if (CubeMesh.Succeeded())
	{
		Mesh->SetStaticMesh(CubeMesh.Object);
		UpButton->SetStaticMesh(CubeMesh.Object);
		DownButton->SetStaticMesh(CubeMesh.Object);
	}

	DigitText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("DigitText"));
	DigitText->SetupAttachment(Root);
	DigitText->SetRelativeLocation(FVector(3.5f, 0.f, 0.f));
	DigitText->SetHorizontalAlignment(EHTA_Center);
	DigitText->SetVerticalAlignment(EVRTA_TextCenter);
	DigitText->SetWorldSize(16.f);
	DigitText->SetTextRenderColor(FColor(255, 220, 0));
	DigitText->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DigitText->SetText(FText::FromString(TEXT("0")));
}

void ACodeDial::Interact_Implementation(AActor* Interactor)
{
	if (Lock)
	{
		Lock->Interact(Interactor);
	}
}

FText ACodeDial::GetPromptText() const
{
	return Lock ? Lock->GetPromptText() : FText::GetEmpty();
}

void ACodeDial::ChangeDigit(int32 Amount)
{
	Digit = ((Digit + Amount) % 10 + 10) % 10;
	DigitText->SetText(FText::AsNumber(Digit));
}

// ---------------- ACodeLock ----------------

ACodeLock::ACodeLock()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));

	// 뒷판: 엔진 기본 큐브로 만든 폭 90, 높이 50 판
	Plate = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plate"));
	Plate->SetupAttachment(Root);
	Plate->SetRelativeScale3D(FVector(0.04f, 0.9f, 0.5f));
	Plate->SetRelativeLocation(FVector(-4.f, 0.f, 0.f));

	// 확인 버튼: 다이얼 오른쪽 (앞에서 볼 때)
	ConfirmButton = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ConfirmButton"));
	ConfirmButton->SetupAttachment(Root);
	ConfirmButton->SetRelativeLocation(FVector(0.f, -36.f, 0.f));
	ConfirmButton->SetRelativeScale3D(FVector(0.06f, 0.12f, 0.12f));

	if (CubeMesh.Succeeded())
	{
		Plate->SetStaticMesh(CubeMesh.Object);
		ConfirmButton->SetStaticMesh(CubeMesh.Object);
	}

	ConfirmText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("ConfirmText"));
	ConfirmText->SetupAttachment(ConfirmButton);
	ConfirmText->SetAbsolute(false, false, true);
	ConfirmText->SetRelativeLocation(FVector(60.f, 0.f, 0.f));
	ConfirmText->SetHorizontalAlignment(EHTA_Center);
	ConfirmText->SetVerticalAlignment(EVRTA_TextCenter);
	ConfirmText->SetWorldSize(8.f);
	ConfirmText->SetTextRenderColor(FColor(120, 255, 120));
	ConfirmText->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ConfirmText->SetText(FText::FromString(TEXT("OK")));

	// 조사 범위는 자물쇠 앞면, 잠금장치 화면은 정면에서 확대
	InteractBox->SetBoxExtent(FVector(10.f, 46.f, 26.f));
	InteractBox->SetRelativeLocation(FVector(6.f, 0.f, 0.f));
	InspectCamera->SetRelativeLocation(FVector(75.f, 0.f, 0.f));
	InspectCamera->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));

	PromptText = FText::FromString(TEXT("잠금장치 조사하기"));
}

void ACodeLock::BeginPlay()
{
	Super::BeginPlay();

	// Code 자릿수만큼 다이얼 생성 (06강 SpawnActor)
	// 앞(+X)에서 볼 때 왼쪽이 첫 번째 자리가 되도록 +Y 쪽부터 배치
	const int32 NumDials = Code.Len();
	for (int32 i = 0; i < NumDials; ++i)
	{
		const FVector LocalOffset(0.f, ((NumDials - 1) * 0.5f - i) * DialSpacing + 6.f, 0.f);
		const FVector SpawnLocation = GetActorTransform().TransformPosition(LocalOffset);

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		ACodeDial* Dial = GetWorld()->SpawnActor<ACodeDial>(ACodeDial::StaticClass(), SpawnLocation, GetActorRotation(), SpawnParams);
		if (Dial)
		{
			// 자물쇠 크기를 줄여 배치해도 다이얼이 같은 비율이 되도록 크기도 맞춤
			Dial->SetActorScale3D(GetActorScale3D());
			Dial->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			Dial->Lock = this;
			Dials.Add(Dial);
		}
	}
}

void ACodeLock::HandleInspectClick(const FHitResult& Hit)
{
	if (bSolved) return;

	if (Hit.GetComponent() == ConfirmButton)
	{
		TryOpen();
		return;
	}

	if (ACodeDial* Dial = Cast<ACodeDial>(Hit.GetActor()))
	{
		// 아래 버튼은 -1, 위 버튼이나 숫자 칸은 +1
		Dial->ChangeDigit(Hit.GetComponent() == Dial->DownButton ? -1 : 1);
	}
}

void ACodeLock::TryOpen()
{
	if (bSolved) return;

	FString Current;
	for (const ACodeDial* Dial : Dials)
	{
		Current.AppendInt(Dial->GetDigit());
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

	OnOpened.Broadcast();
}
