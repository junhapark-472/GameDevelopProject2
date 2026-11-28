#include "StoryTrigger.h"
#include "StoryEvent.h"
#include "GSD2FunctionLibrary.h"
#include "Components/BoxComponent.h"

AStoryTrigger::AStoryTrigger()
{
	PrimaryActorTick.bCanEverTick = true;

	// 라인 트레이스(Visibility)만 막는 상자: 플레이어 이동은 막지 않는다
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = Box;
	Box->SetBoxExtent(FVector(20.f, 20.f, 20.f));
	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Box->SetCollisionResponseToAllChannels(ECR_Ignore);
	Box->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	PromptText = FText::FromString(TEXT("조사하기"));
}

bool AStoryTrigger::IsActive() const
{
	if (bUsed) return false;
	if (!UGSD2FunctionLibrary::HasProgressFlag(this, RequiredFlag)) return false;
	if (!DoneFlag.IsNone() && UGSD2FunctionLibrary::HasProgressFlag(this, DoneFlag)) return false;
	return true;
}

void AStoryTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 조사할 수 없는 동안에는 조준되지 않게 충돌을 끈다
	const ECollisionEnabled::Type Wanted = IsActive() ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision;
	if (Box->GetCollisionEnabled() != Wanted)
	{
		Box->SetCollisionEnabled(Wanted);
	}
}

void AStoryTrigger::Interact_Implementation(AActor* Interactor)
{
	if (!IsActive()) return;

	bUsed = true;
	if (EventToPlay)
	{
		EventToPlay->Play();
	}
}

FText AStoryTrigger::GetPromptText() const
{
	return IsActive() ? PromptText : FText::GetEmpty();
}
