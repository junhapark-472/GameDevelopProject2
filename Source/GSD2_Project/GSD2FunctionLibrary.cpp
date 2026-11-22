#include "GSD2FunctionLibrary.h"
#include "GSD2GameInstance.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/SceneComponent.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

void UGSD2FunctionLibrary::ShowMessage(const FString& Message, FLinearColor Color, float Duration)
{
	if (GEngine && !Message.IsEmpty())
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color.ToFColor(true), Message);
	}
}

void UGSD2FunctionLibrary::FaceCamera(const UObject* WorldContextObject, USceneComponent* Component)
{
	if (!Component) return;

	const APlayerCameraManager* Camera = UGameplayStatics::GetPlayerCameraManager(WorldContextObject, 0);
	if (!Camera) return;

	const FVector ToCamera = Camera->GetCameraLocation() - Component->GetComponentLocation();
	Component->SetWorldRotation(FRotator(0.f, ToCamera.Rotation().Yaw, 0.f));
}

void UGSD2FunctionLibrary::SetProgressFlag(const UObject* WorldContextObject, FName Flag)
{
	if (Flag.IsNone()) return;

	if (UGSD2GameInstance* GI = Cast<UGSD2GameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject)))
	{
		GI->SetFlag(Flag);
	}
}

bool UGSD2FunctionLibrary::HasProgressFlag(const UObject* WorldContextObject, FName Flag)
{
	if (Flag.IsNone()) return true;

	const UGSD2GameInstance* GI = Cast<UGSD2GameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	return GI ? GI->HasFlag(Flag) : true;
}

void UGSD2FunctionLibrary::UpdateObjectiveAndHint(const UObject* WorldContextObject, const FText& Objective, const FText& Hint)
{
	UGSD2GameInstance* GI = Cast<UGSD2GameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	if (!GI) return;

	if (!Objective.IsEmpty())
	{
		GI->SetObjective(Objective);
	}
	if (!Hint.IsEmpty())
	{
		GI->SetHint(Hint);
	}
}
