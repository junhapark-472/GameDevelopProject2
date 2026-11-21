// Copyright Epic Games, Inc. All Rights Reserved.


#include "GSD2_ProjectPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "GSD2_ProjectCameraManager.h"
#include "Blueprint/UserWidget.h"
#include "GSD2_Project.h"
#include "Widgets/Input/SVirtualJoystick.h"
#include "GameHUDWidget.h"
#include "InteractionComponent.h"

AGSD2_ProjectPlayerController::AGSD2_ProjectPlayerController()
{
	// set the player camera manager class
	PlayerCameraManagerClass = AGSD2_ProjectCameraManager::StaticClass();
}

void AGSD2_ProjectPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 게임 화면 HUD (조준점, 상호작용 안내) 생성
	if (IsLocalPlayerController() && BP_HUDWidget)
	{
		HUDWidget = CreateWidget<UGameHUDWidget>(this, BP_HUDWidget);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
		}
	}

	
	// only spawn touch controls on local player controllers
	if (IsLocalPlayerController() && ShouldUseTouchControls())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogGSD2_Project, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}
}

void AGSD2_ProjectPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (!HUDWidget) return;

	// 캐릭터의 InteractionComponent 로 지금 바라보는 대상을 찾아 HUD 에 전달
	const APawn* ControlledPawn = GetPawn();
	const UInteractionComponent* Interaction = ControlledPawn ? ControlledPawn->FindComponentByClass<UInteractionComponent>() : nullptr;
	HUDWidget->UpdateInteractPrompt(Interaction ? Interaction->FindInteractable() : nullptr);
}

void AGSD2_ProjectPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Context
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
	
}

bool AGSD2_ProjectPlayerController::ShouldUseTouchControls() const
{
	// are we on a mobile platform? Should we force touch?
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}
