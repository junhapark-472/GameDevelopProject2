#include "GSD2FunctionLibrary.h"
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
