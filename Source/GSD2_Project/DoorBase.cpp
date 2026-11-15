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
}

void ADoorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 목표 각도까지 부드럽게 회전
	const float TargetAngle = bIsOpen ? OpenAngle : 0.f;
	if (!FMath::IsNearlyEqual(CurrentAngle, TargetAngle, 0.01f))
	{
		CurrentAngle = FMath::FInterpTo(CurrentAngle, TargetAngle, DeltaTime, OpenSpeed);
		Hinge->SetRelativeRotation(FRotator(0.f, CurrentAngle, 0.f));
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
	return bIsOpen ? FText::FromString(TEXT("문 닫기")) : PromptText;
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
