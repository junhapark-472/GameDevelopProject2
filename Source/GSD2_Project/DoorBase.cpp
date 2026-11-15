#include "DoorBase.h"
#include "InventoryComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/Engine.h"
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

	// 기본 모양: 폭 100, 높이 200, 두께 10 인 판 (한쪽 끝이 Hinge 에 붙음)
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		DoorMesh->SetStaticMesh(CubeMesh.Object);
	}
	DoorMesh->SetRelativeLocation(FVector(0.f, 50.f, 100.f));
	DoorMesh->SetRelativeScale3D(FVector(0.1f, 1.f, 2.f));
}

void ADoorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float Target = bIsOpen ? OpenAngle : 0.f;
	if (!FMath::IsNearlyEqual(CurrentAngle, Target, 0.01f))
	{
		CurrentAngle = FMath::FInterpTo(CurrentAngle, Target, DeltaTime, OpenSpeed);
		Hinge->SetRelativeRotation(FRotator(0.f, CurrentAngle, 0.f));
	}
}

void ADoorBase::Interact_Implementation(AActor* Interactor)
{
	if (bLocked)
	{
		UInventoryComponent* Inventory = Interactor ? Interactor->FindComponentByClass<UInventoryComponent>() : nullptr;

		if (!RequiredItemID.IsNone() && Inventory && Inventory->HasItem(RequiredItemID))
		{
			if (bConsumeKey)
			{
				Inventory->RemoveItem(RequiredItemID);
			}
			Unlock();
		}
		else
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, LockedMessage);
			}
			return;
		}
	}

	ToggleDoor();
}

void ADoorBase::Unlock()
{
	if (!bLocked) return;

	bLocked = false;
	OnUnlocked.Broadcast();
}

void ADoorBase::ToggleDoor()
{
	bIsOpen = !bIsOpen;
}
