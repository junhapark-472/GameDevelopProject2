#include "InspectStation.h"
#include "GSD2FunctionLibrary.h"
#include "GSD2_ProjectPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"

AInspectStation::AInspectStation()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	DisplayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DisplayMesh"));
	DisplayMesh->SetupAttachment(Root);

	// 라인 트레이스(Visibility)만 막는 상자: 플레이어 이동은 막지 않는다
	InteractBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractBox"));
	InteractBox->SetupAttachment(Root);
	InteractBox->SetBoxExtent(FVector(40.f, 40.f, 30.f));
	InteractBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	InspectCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("InspectCamera"));
	InspectCamera->SetupAttachment(Root);
	InspectCamera->SetRelativeLocation(FVector(-100.f, 0.f, 100.f));
	InspectCamera->SetRelativeRotation(FRotator(-40.f, 0.f, 0.f));

	PromptText = FText::FromString(TEXT("조사하기"));
}

void AInspectStation::Interact_Implementation(AActor* Interactor)
{
	if (!CanInspect()) return;

	const APawn* Pawn = Cast<APawn>(Interactor);
	AGSD2_ProjectPlayerController* PC = Pawn ? Cast<AGSD2_ProjectPlayerController>(Pawn->GetController()) : nullptr;
	if (PC)
	{
		PC->StartInspection(this);
	}
}

FText AInspectStation::GetPromptText() const
{
	// 조사할 수 없는 상태면 안내 문구를 띄우지 않는다
	return CanInspect() ? PromptText : FText::GetEmpty();
}

void AInspectStation::OnInspectStart()
{
	bInspecting = true;
	InteractBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (!bInspectedOnce)
	{
		bInspectedOnce = true;
		UGSD2FunctionLibrary::ShowMessage(FirstInspectMessage);
		UGSD2FunctionLibrary::SetProgressFlag(this, InspectFlag);
	}
}

void AInspectStation::OnInspectEnd()
{
	bInspecting = false;
	InteractBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}
