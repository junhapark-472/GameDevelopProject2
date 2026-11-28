#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PhoneWidget.generated.h"

class AStoryEvent;
class UButton;
class UTextBlock;
class UVerticalBox;

// 13씬 휴대폰 화면 (08강 UMG)
// 전원 → (배터리 없음) → 보조배터리 연결 → 충전 → 잠금 화면 → 긴급전화 → 112 입력 → 통화
UCLASS()
class GSD2_PROJECT_API UPhoneWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Setup(AStoryEvent* InOwnerEvent, bool bInHasBattery);

protected:
	UFUNCTION()
	void OnPowerClicked();

	UFUNCTION()
	void OnBatteryClicked();

	UFUNCTION()
	void OnEmergencyClicked();

	UFUNCTION()
	void OnDigitClicked();

	UFUNCTION()
	void OnDeleteClicked();

	UFUNCTION()
	void OnCallClicked();

private:
	void ShowLockScreen();
	void ClearButtons();
	UButton* AddButton(const FString& Label);
	UTextBlock* MakeText(const FString& Text, float FontSize);

	UPROPERTY(Transient)
	TObjectPtr<AStoryEvent> OwnerEvent;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ScreenText;

	UPROPERTY(Transient)
	TObjectPtr<UVerticalBox> ButtonArea;

	// 숫자 버튼 (0~9)
	UPROPERTY(Transient)
	TArray<TObjectPtr<UButton>> DigitButtons;

	FString DialNumber;
	bool bHasBattery = false;
	FTimerHandle ChargeTimer;
};
