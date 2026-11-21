#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameHUDWidget.generated.h"

class AInteractableBase;
class UTextBlock;

// 게임 화면 HUD (08강 UMG). 위젯 블루프린트 WBP_HUD 의 부모 클래스
// - 화면 가운데 조준점(+), 상호작용 가능한 대상을 보고 있으면 "F - 행동" 표시
UCLASS()
class GSD2_PROJECT_API UGameHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 조준 대상에 맞춰 조준점 색과 안내 문구 갱신 (대상이 없으면 nullptr)
	void UpdateInteractPrompt(const AInteractableBase* Target);

	// 위젯 블루프린트에 같은 이름의 Text 가 있어야 함
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CrosshairText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* InteractPromptText;
};
