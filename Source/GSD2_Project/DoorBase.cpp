#include "DoorBase.h"
#include "GSD2FunctionLibrary.h"
#include "InventoryComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
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
		return FText::FromString(TEXT("잠겨 있음"));
	}
	return bIsOpen ? ClosePromptText : PromptText;
}

void ADoorBase::Unlock(bool bAlsoOpen)
{
	if (bLocked)
	{
		bLocked = false;
		OnUnlocked.Broadcast();
	}

	if (bAlsoOpen && !bIsOpen)
	{
		ToggleDoor();
	}
}

void ADoorBase::ToggleDoor()
{
	bIsOpen = !bIsOpen;
}
