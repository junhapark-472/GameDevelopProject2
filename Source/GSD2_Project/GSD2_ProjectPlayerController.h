// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GSD2_ProjectPlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;
class UGameHUDWidget;

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

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

	/** Returns true if the player should use UMG touch controls */
	bool ShouldUseTouchControls() const;
};
