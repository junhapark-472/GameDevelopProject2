#include "CodeLock.h"
#include "DoorBase.h"
#include "Components/ChildActorComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

// ---------------- ACodeDial ----------------

ACodeDial::ACodeDial()
{
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// 기본 모양: 폭 12, 높이 18, 두께 6 인 블록 (앞면이 +X)
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

	// 뒷판: 폭 64, 높이 26
	Plate = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plate"));
	Plate->SetupAttachment(Root);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		Plate->SetStaticMesh(CubeMesh.Object);
	}
	Plate->SetRelativeScale3D(FVector(0.04f, 0.64f, 0.26f));
	Plate->SetRelativeLocation(FVector(-4.f, 0.f, 0.f));

	// 다이얼 4개 (가로로 14cm 간격)
	const int32 NumDials = 4;
	const float Spacing = 14.f;
	for (int32 i = 0; i < NumDials; ++i)
	{
		UChildActorComponent* DialComp = CreateDefaultSubobject<UChildActorComponent>(*FString::Printf(TEXT("Dial%d"), i + 1));
		DialComp->SetupAttachment(Root);
		DialComp->SetChildActorClass(ACodeDial::StaticClass());
		// 앞(+X)에서 볼 때 왼쪽이 첫 번째 자리가 되도록 (+Y 쪽부터 배치)
		DialComp->SetRelativeLocation(FVector(0.f, ((NumDials - 1) * 0.5f - i) * Spacing, 0.f));
		DialComponents.Add(DialComp);
	}
}

void ACodeLock::BeginPlay()
{
	Super::BeginPlay();

	Dials.Reset();
	for (UChildActorComponent* DialComp : DialComponents)
	{
		if (ACodeDial* Dial = DialComp ? Cast<ACodeDial>(DialComp->GetChildActor()) : nullptr)
		{
			Dial->Lock = this;
			Dial->SetDigit(0);
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
	if (GEngine && !SolvedMessage.IsEmpty())
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, SolvedMessage);
	}

	if (DoorToUnlock)
	{
		DoorToUnlock->Unlock();
		if (bOpenDoorOnSolved && !DoorToUnlock->bIsOpen)
		{
			DoorToUnlock->ToggleDoor();
		}
	}

	OnOpened.Broadcast();
}
