#include "DoorBase.h"
#include "GSD2FunctionLibrary.h"
#include "StoryEvent.h"
#include "InventoryComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "UObject/ConstructorHelpers.h"

ADoorBase::ADoorBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Hinge = CreateDefaultSubobject<USceneComponent>(TEXT("Hinge"));
	Hinge->SetupAttachment(Root);

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(Hinge);

	// 기본 모양: 엔진 기본 큐브로 만든 폭 100, 높이 200, 두께 10 판 (한쪽 끝이 Hinge 에 붙음)
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		DoorMesh->SetStaticMesh(CubeMesh.Object);
	}
	DoorMesh->SetRelativeLocation(FVector(0.f, 50.f, 100.f));
	DoorMesh->SetRelativeScale3D(FVector(0.1f, 1.f, 2.f));

	PromptText = FText::FromString(TEXT("문 열기"));
	ClosePromptText = FText::FromString(TEXT("문 닫기"));
	LockedPromptText = FText::FromString(TEXT("잠겨 있음"));
}

void ADoorBase::BeginPlay()
{
	Super::BeginPlay();

	if (AttachedActorNames.Num() == 0) return;

	// 이름이 같은 액터를 찾아 문/서랍에 붙인다 (열면 같이 움직임)
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(this, AActor::StaticClass(), FoundActors);
	for (AActor* Actor : FoundActors)
	{
		if (Actor && Actor->GetRootComponent() && AttachedActorNames.Contains(Actor->GetFName()))
		{
			// 맵에 고정(Static)으로 놓인 액터는 움직일 수 있게 바꾼 뒤 붙인다
			Actor->GetRootComponent()->SetMobility(EComponentMobility::Movable);
			Actor->AttachToComponent(DoorMesh, FAttachmentTransformRules::KeepWorldTransform);

			// 붙인 장식(문짝 등)이 상호작용 레이를 가리지 않게 한다 (줍는 아이템은 제외)
			if (Actor->IsA<AStaticMeshActor>())
			{
				Cast<AStaticMeshActor>(Actor)->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
			}
		}
	}
}

void ADoorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 열림 정도(0~1)를 부드럽게 바꾸고, 그만큼 회전하거나 밀려 나온다
	const float TargetAlpha = bIsOpen ? 1.f : 0.f;
	if (!FMath::IsNearlyEqual(OpenAlpha, TargetAlpha, 0.001f))
	{
		OpenAlpha = FMath::FInterpTo(OpenAlpha, TargetAlpha, DeltaTime, OpenSpeed);

		if (bSlide)
		{
			Hinge->SetRelativeLocation(SlideOffset * OpenAlpha);
		}
		else
		{
			Hinge->SetRelativeRotation(FRotator(0.f, OpenAngle * OpenAlpha, 0.f));
		}
	}
}

void ADoorBase::Interact_Implementation(AActor* Interactor)
{
	if (bLocked)
	{
		UInventoryComponent* Inventory = Interactor ? Interactor->FindComponentByClass<UInventoryComponent>() : nullptr;
		const bool bHasKey = !RequiredItemID.IsNone() && Inventory && Inventory->HasItem(RequiredItemID);

		if (!bHasKey)
		{
			UGSD2FunctionLibrary::ShowMessage(LockedMessage, FLinearColor::Red, 2.f);
			return;
		}

		if (bConsumeKey)
		{
			Inventory->RemoveItem(RequiredItemID);
		}
		Unlock();
	}

	ToggleDoor();
}

FText ADoorBase::GetPromptText() const
{
	if (bLocked)
	{
		return LockedPromptText;
	}
	if (bIsOpen && bStayOpen)
	{
		return FText::GetEmpty();
	}
	return bIsOpen ? ClosePromptText : PromptText;
}

void ADoorBase::Unlock(bool bAlsoOpen)
{
	if (bLocked)
	{
		bLocked = false;
		UGSD2FunctionLibrary::UpdateObjectiveAndHint(this, UnlockObjective, UnlockHint);
		OnUnlocked.Broadcast();
	}

	if (bAlsoOpen && !bIsOpen)
	{
		ToggleDoor();
	}
}

void ADoorBase::ToggleDoor()
{
	if (bIsOpen && bStayOpen) return;

	bIsOpen = !bIsOpen;

	if (bIsOpen && OpenEvent && !bOpenEventPlayed)
	{
		bOpenEventPlayed = true;
		OpenEvent->Play();
	}

	// 한 번 열면 그대로 두는 경우: 라인 트레이스(Visibility)가 통과하도록 해서 안의 물건을 조준할 수 있게
	if (bIsOpen && bStayOpen)
	{
		DoorMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	}
}
