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
#include "InspectStation.h"
#include "Components/InputComponent.h"
#include "GameFramework/Pawn.h"

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

	// 조사 화면 중에는 조준점/안내 문구를 숨긴다
	HUDWidget->SetVisibility(IsInspecting() ? ESlateVisibility::Hidden : ESlateVisibility::HitTestInvisible);
	if (IsInspecting()) return;

	// 캐릭터의 InteractionComponent 로 지금 바라보는 대상을 찾아 HUD 에 전달
	const APawn* ControlledPawn = GetPawn();
	const UInteractionComponent* Interaction = ControlledPawn ? ControlledPawn->FindComponentByClass<UInteractionComponent>() : nullptr;
	HUDWidget->UpdateInteractPrompt(Interaction ? Interaction->FindInteractable() : nullptr);
}

void AGSD2_ProjectPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// 조사 화면 조작 (04강 키 입력 바인딩). 다른 입력을 막지 않도록 bConsumeInput = false
	if (InputComponent)
	{
		InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &AGSD2_ProjectPlayerController::OnInspectClick).bConsumeInput = false;
		InputComponent->BindKey(EKeys::Escape, IE_Pressed, this, &AGSD2_ProjectPlayerController::OnInspectExit).bConsumeInput = false;
		InputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &AGSD2_ProjectPlayerController::OnInspectExit).bConsumeInput = false;
	}

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

void AGSD2_ProjectPlayerController::StartInspection(AInspectStation* Station)
{
	if (!Station || CurrentStation) return;

	CurrentStation = Station;

	// 오브젝트의 카메라로 시점 전환 + 이동/시점 회전 막기 + 마우스 커서 표시
	SetViewTargetWithBlend(Station, Station->BlendTime);
	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);
	bShowMouseCursor = true;

	// 조사 카메라에 내 캐릭터 몸이 가리지 않도록 잠시 숨김
	if (APawn* MyPawn = GetPawn())
	{
		MyPawn->SetActorHiddenInGame(true);
	}

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);

	Station->OnInspectStart();
}

void AGSD2_ProjectPlayerController::EndInspection()
{
	AInspectStation* Station = CurrentStation;
	if (!Station) return;

	CurrentStation = nullptr;

	// 1인칭 시점으로 복귀
	SetViewTargetWithBlend(GetPawn(), Station->BlendTime);
	ResetIgnoreMoveInput();
	ResetIgnoreLookInput();
	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());

	if (APawn* MyPawn = GetPawn())
	{
		MyPawn->SetActorHiddenInGame(false);
	}

	Station->OnInspectEnd();
}

void AGSD2_ProjectPlayerController::OnInspectClick()
{
	if (!CurrentStation) return;

	// 마우스 커서 위치로 라인 트레이스 (05강) 해서 맞은 물체를 조사 오브젝트에 전달
	FHitResult Hit;
	if (GetHitResultUnderCursor(ECC_Visibility, false, Hit))
	{
		CurrentStation->HandleInspectClick(Hit);
	}
}

void AGSD2_ProjectPlayerController::OnInspectExit()
{
	if (CurrentStation)
	{
		EndInspection();
	}
}
