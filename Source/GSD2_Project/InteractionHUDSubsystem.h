#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "InteractionHUDSubsystem.generated.h"

class UCanvas;
class APlayerController;

// 화면 가운데 조준점(+) 표시
// 상호작용 가능한 물체(InteractableBase)를 보고 있으면 초록색으로 바뀌고 [F] 표시
// 레벨에 따로 배치할 필요 없이 게임 실행 시 자동으로 동작한다
UCLASS()
class GSD2_PROJECT_API UInteractionHUDSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Deinitialize() override;

private:
	void Draw(UCanvas* Canvas, APlayerController* PC);

	FDelegateHandle DrawHandle;
};
