// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GSD2_ProjectPlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;
class UGameHUDWidget;
class AInspectStation;

/**
 *  Simple first person Player Controller
 *  Manages the input mapping context.
 *  Overrides the Player Camera Manager class.
 */
UCLASS(abstract, config="Game")
class GSD2_PROJECT_API AGSD2_ProjectPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	/** Constructor */
	AGSD2_ProjectPlayerController();

	/** 조사 시점 시작: 해당 오브젝트 카메라로 전환하고 마우스 커서 표시 */
	void StartInspection(AInspectStation* Station);

	/** 조사 시점 종료: 1인칭 시점으로 복귀 */
	void EndInspection();

	/** 조사 화면이 열려 있는지 */
	bool IsInspecting() const { return CurrentStation != nullptr; }

protected:

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	/** Mobile controls widget to spawn */
	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	/** Pointer to the mobile controls widget */
	UPROPERTY()
	TObjectPtr<UUserWidget> MobileControlsWidget;

	/** If true, the player will use UMG touch controls even if not playing on mobile platforms */
	UPROPERTY(EditAnywhere, Config, Category = "Input|Touch Controls")
	bool bForceTouchControls = false;

	/** 게임 화면 HUD 위젯 블루프린트 (WBP_HUD) - 블루프린트 플레이어 컨트롤러에서 지정 */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGameHUDWidget> BP_HUDWidget;

	/** 화면에 추가된 HUD 위젯 */
	UPROPERTY()
	TObjectPtr<UGameHUDWidget> HUDWidget;

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** 매 프레임: 조준 중인 상호작용 대상을 HUD 에 표시 */
	virtual void PlayerTick(float DeltaTime) override;

	/** 조사 화면에서 마우스 왼쪽 클릭 */
	void OnInspectClick();

	/** 조사 화면에서 ESC / 마우스 오른쪽 */
	void OnInspectExit();

	/** 지금 조사 중인 오브젝트 */
	UPROPERTY()
	TObjectPtr<AInspectStation> CurrentStation;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

	/** Returns true if the player should use UMG touch controls */
	bool ShouldUseTouchControls() const;
};
