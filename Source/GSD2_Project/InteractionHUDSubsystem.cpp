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
#include "EngineUtils.h"
#include "HAL/IConsoleManager.h"
#include "UObject/UnrealType.h"

// 테스트용 이름표 표시 여부 (콘솔에서 gsd.ShowDebugLabels 0 으로 끌 수 있음)
static TAutoConsoleVariable<int32> CVarShowDebugLabels(
	TEXT("gsd.ShowDebugLabels"),
	1,
	TEXT("1: 'DebugLabel' 문자열 변수가 있는 액터 위에 이름표 표시, 0: 끄기"));

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

	if (CVarShowDebugLabels.GetValueOnGameThread() != 0)
	{
		DrawDebugLabels(Canvas, PC, Start);
	}

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

void UInteractionHUDSubsystem::DrawDebugLabels(UCanvas* Canvas, APlayerController* PC, const FVector& CameraLocation)
{
	UWorld* World = GetWorld();
	if (!World || !GEngine) return;

	const float MaxDistanceSq = FMath::Square(800.f);
	UFont* Font = GEngine->GetSmallFont();

	FFontRenderInfo RenderInfo;
	RenderInfo.bEnableShadow = true;

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;

		// C++ 클래스든 블루프린트든 "DebugLabel" 이라는 문자열 변수가 있으면 표시
		const FStrProperty* LabelProp = FindFProperty<FStrProperty>(Actor->GetClass(), TEXT("DebugLabel"));
		if (!LabelProp) continue;

		const FString Label = LabelProp->GetPropertyValue_InContainer(Actor);
		if (Label.IsEmpty()) continue;

		FVector Origin, Extent;
		Actor->GetActorBounds(true, Origin, Extent);
		const FVector LabelLocation = Origin + FVector(0.f, 0.f, Extent.Z + 15.f);

		if (FVector::DistSquared(CameraLocation, LabelLocation) > MaxDistanceSq) continue;

		FVector2D ScreenPos;
		if (!PC->ProjectWorldLocationToScreen(LabelLocation, ScreenPos, true)) continue;

		float TextW = 0.f, TextH = 0.f;
		Canvas->StrLen(Font, Label, TextW, TextH);

		Canvas->SetDrawColor(FColor(255, 230, 120));
		Canvas->DrawText(Font, Label, ScreenPos.X - TextW * 0.5f, ScreenPos.Y - TextH, 1.f, 1.f, RenderInfo);
	}
}
