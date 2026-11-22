#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CodeLockWidget.generated.h"

class ACodeLock;
class UButton;
class UTextBlock;

// 숫자 자물쇠 입력 화면 (08강 UMG)
// - 자릿수마다 [▲ / 숫자 / ▼] 를 만들고, 아래에 [확인] 버튼
// - 자물쇠마다 자릿수가 달라질 수 있어서 화면 구성은 C++ 에서 만든다 (WidgetTree->ConstructWidget)
UCLASS()
class GSD2_PROJECT_API UCodeLockWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 자물쇠를 연결하고 자릿수만큼 버튼/숫자 칸을 만든다 (AddToViewport 전에 호출)
	void Setup(ACodeLock* InLock);

	// 자물쇠의 현재 숫자를 화면에 다시 표시
	void RefreshDigits();

protected:
	// 버튼 클릭 이벤트 (OnClicked.AddDynamic 으로 연결하려면 UFUNCTION 이어야 함)
	UFUNCTION()
	void OnUpClicked();

	UFUNCTION()
	void OnDownClicked();

	UFUNCTION()
	void OnConfirmClicked();

private:
	UButton* MakeButton(const FString& Label, float FontSize);
	UTextBlock* MakeText(const FString& Text, float FontSize);

	UPROPERTY(Transient)
	TObjectPtr<ACodeLock> Lock;

	// i 번째 자리의 버튼/숫자
	UPROPERTY(Transient)
	TArray<TObjectPtr<UButton>> UpButtons;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UButton>> DownButtons;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UTextBlock>> DigitTexts;
};
