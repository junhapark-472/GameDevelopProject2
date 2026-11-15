#include "PuzzleSlot.h"
#include "PuzzleManager.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"

APuzzleSlot::APuzzleSlot()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

	// 기본 모양: 지름 50, 두께 2 인 납작한 원판
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		Mesh->SetStaticMesh(CylinderMesh.Object);
	}
	Mesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.02f));
	Mesh->SetRelativeLocation(FVector(0.f, 0.f, 1.f));

	Label = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Label"));
	Label->SetupAttachment(Root);
	Label->SetWorldSize(26.f);
	Label->SetHorizontalAlignment(EHTA_Center);
	Label->SetVerticalAlignment(EVRTA_TextBottom);
	Label->SetTextRenderColor(FColor(0, 230, 255));
	Label->SetRelativeLocation(FVector(0.f, 0.f, 95.f)); // 조각 이름표보다 위
	Label->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APuzzleSlot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 글씨가 항상 플레이어 카메라 쪽을 바라보게
	if (Label && Label->IsVisible())
	{
		if (const APlayerCameraManager* Cam = UGameplayStatics::GetPlayerCameraManager(this, 0))
		{
			const FVector ToCam = Cam->GetCameraLocation() - Label->GetComponentLocation();
			Label->SetWorldRotation(FRotator(0.f, ToCam.Rotation().Yaw, 0.f));
		}
	}
}

void APuzzleSlot::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (Label)
	{
		Label->SetText(FText::FromString(TEXT("#") + SlotName.ToString()));
		Label->SetVisibility(bShowLabel && !SlotName.IsNone());
	}
}

void APuzzleSlot::Interact_Implementation(AActor* Interactor)
{
	if (Manager)
	{
		Manager->PlaceSelectedOn(this);
	}
}

FVector APuzzleSlot::GetPlaceLocation() const
{
	return GetActorLocation() + FVector(0.f, 0.f, 2.f);
}
