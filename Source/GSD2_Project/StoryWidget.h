#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StoryWidget.generated.h"

class AStoryEvent;
class UBorder;
class UButton;
class UTextBlock;
class UVerticalBox;

// 이야기 이벤트 화면 (08강 UMG)
// - 화면 하단 대사창 (이름 + 대사)
// - 대사창 위 오른쪽 선택지 버튼
// - 검은 화면 (암전) + 엔딩 제목/버튼
// 화면 구성은 C++ 에서 만든다 (WidgetTree->ConstructWidget)
UCLASS()
class GSD2_PROJECT_API UStoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Setup(AStoryEvent* InOwnerEvent);

	void ShowLine(const FText& Speaker, const FText& Text);
	void SetBlack(bool bBlack);
	void ShowChoices(const FText& ChoiceA, const FText& ChoiceB);
	void ShowEnding(const FText& Title, bool bCanRetry);

protected:
	UFUNCTION()
	void OnChoiceAClicked();

	UFUNCTION()
	void OnChoiceBClicked();

	UFUNCTION()
	void OnEndingFirstClicked();

	UFUNCTION()
	void OnEndingQuitClicked();

private:
	UButton* MakeButton(const FText& Label, float FontSize);
	UTextBlock* MakeText(const FText& Text, float FontSize);

	UPROPERTY(Transient)
	TObjectPtr<AStoryEvent> OwnerEvent;

	UPROPERTY(Transient)
	TObjectPtr<UBorder> BlackScreen;

	UPROPERTY(Transient)
	TObjectPtr<UBorder> DialogueBox;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> SpeakerText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> LineText;

	UPROPERTY(Transient)
	TObjectPtr<UVerticalBox> ChoiceBox;

	UPROPERTY(Transient)
	TObjectPtr<UVerticalBox> EndingBox;
};
