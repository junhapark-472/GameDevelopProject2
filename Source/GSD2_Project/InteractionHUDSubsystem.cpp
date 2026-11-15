#include "InteractionHUDSubsystem.h"
#include "InteractableBase.h"
#include "InteractionComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Debug/DebugDrawService.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

bool UInteractionHUDSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer)) return false;

	const UWorld* World = Cast<UWorld>(Outer);
	return World && World->IsGameWorld();
}

void UInteractionHUDSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	DrawHandle = UDebugDrawService::Register(TEXT("Game"),
		FDebugDrawDelegate::CreateUObject(this, &UInteractionHUDSubsystem::Draw));
}

void UInteractionHUDSubsystem::Deinitialize()
{
	if (DrawHandle.IsValid())
	{
		UDebugDrawService::Unregister(DrawHandle);
		DrawHandle.Reset();
	}
	Super::Deinitialize();
}

void UInteractionHUDSubsystem::Draw(UCanvas* Canvas, APlayerController* InPC)
{
	UWorld* World = GetWorld();
	if (!Canvas || !World) return;
	if (InPC && InPC->GetWorld() != World) return;

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC || !PC->PlayerCameraManager) return;

	APawn* Pawn = PC->GetPawn();

	// InteractionComponent 와 같은 거리/방식으로 검사
	float Distance = 250.f;
	if (Pawn)
	{
		if (const UInteractionComponent* Interaction = Pawn->FindComponentByClass<UInteractionComponent>())
		{
			Distance = Interaction->InteractDistance;
		}
	}

	const FVector Start = PC->PlayerCameraManager->GetCameraLocation();
	const FVector End = Start + PC->PlayerCameraManager->GetCameraRotation().Vector() * Distance;

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(InteractionHUD), false, Pawn);
	const bool bCanInteract = World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params)
		&& Cast<AInteractableBase>(Hit.GetActor()) != nullptr;

	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
	const FLinearColor Color = bCanInteract ? FLinearColor(0.2f, 1.f, 0.3f, 1.f) : FLinearColor(1.f, 1.f, 1.f, 0.8f);
	const float Size = bCanInteract ? 10.f : 6.f;

	Canvas->K2_DrawLine(Center - FVector2D(Size, 0.f), Center + FVector2D(Size, 0.f), 2.f, Color);
	Canvas->K2_DrawLine(Center - FVector2D(0.f, Size), Center + FVector2D(0.f, Size), 2.f, Color);

	if (bCanInteract && GEngine)
	{
		Canvas->SetDrawColor(FColor(80, 255, 100));
		Canvas->DrawText(GEngine->GetMediumFont(), TEXT("[F]"), Center.X + 16.f, Center.Y - 10.f);
	}
}
