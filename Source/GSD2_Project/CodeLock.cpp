#include "CodeLock.h"
#include "DoorBase.h"
#include "GSD2FunctionLibrary.h"
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

	// 기본 모양: 엔진 기본 큐브로 만든 폭 12, 높이 18, 두께 6 블록 (앞면이 +X)
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		Mesh->SetStaticMesh(CubeMesh.Object);
	}
	Mesh->SetRelativeScale3D(FVector(0.06f, 0.12f, 0.18f));

	DigitText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("DigitText"));
	DigitText->SetupAttachment(Root);
	DigitText->SetRelativeLocation(FVector(3.5f, 0.f, 0.f));
	DigitText->SetHorizontalAlignment(EHTA_Center);
	DigitText->SetVerticalAlignment(EVRTA_TextCenter);
	DigitText->SetWorldSize(16.f);
	DigitText->SetTextRenderColor(FColor(255, 220, 0));
	DigitText->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DigitText->SetText(FText::FromString(TEXT("0")));

	PromptText = FText::FromString(TEXT("숫자 돌리기"));
}

void ACodeDial::Interact_Implementation(AActor* Interactor)
{
	if (Lock && Lock->bSolved) return;

	SetDigit((Digit + 1) % 10);

	if (Lock)
	{
		Lock->OnDialChanged();
	}
}

void ACodeDial::SetDigit(int32 NewDigit)
{
	Digit = FMath::Clamp(NewDigit, 0, 9);
	DigitText->SetText(FText::AsNumber(Digit));
}

// ---------------- ACodeLock ----------------

ACodeLock::ACodeLock()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// 뒷판: 엔진 기본 큐브로 만든 폭 64, 높이 26 판
	Plate = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plate"));
	Plate->SetupAttachment(Root);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		Plate->SetStaticMesh(CubeMesh.Object);
	}
	Plate->SetRelativeScale3D(FVector(0.04f, 0.64f, 0.26f));
	Plate->SetRelativeLocation(FVector(-4.f, 0.f, 0.f));
}

void ACodeLock::BeginPlay()
{
	Super::BeginPlay();

	// Code 자릿수만큼 다이얼 생성 (06강 SpawnActor)
	// 앞(+X)에서 볼 때 왼쪽이 첫 번째 자리가 되도록 +Y 쪽부터 배치
	const int32 NumDials = Code.Len();
	for (int32 i = 0; i < NumDials; ++i)
	{
		const FVector LocalOffset(0.f, ((NumDials - 1) * 0.5f - i) * DialSpacing, 0.f);
		const FVector SpawnLocation = GetActorTransform().TransformPosition(LocalOffset);

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		ACodeDial* Dial = GetWorld()->SpawnActor<ACodeDial>(ACodeDial::StaticClass(), SpawnLocation, GetActorRotation(), SpawnParams);
		if (Dial)
		{
			Dial->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			Dial->Lock = this;
			Dials.Add(Dial);
		}
	}
}

void ACodeLock::OnDialChanged()
{
	if (bSolved) return;

	FString Current;
	for (const ACodeDial* Dial : Dials)
	{
		Current.AppendInt(Dial->GetDigit());
	}

	if (Current != Code) return;

	bSolved = true;
	UGSD2FunctionLibrary::ShowMessage(SolvedMessage, FLinearColor::Green);

	if (DoorToUnlock)
	{
		DoorToUnlock->Unlock(bOpenDoorOnSolved);
	}

	OnOpened.Broadcast();
}
