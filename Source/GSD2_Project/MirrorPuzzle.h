#pragma once

#include "CoreMinimal.h"
#include "InspectStation.h"
#include "MirrorPuzzle.generated.h"

class UStaticMeshComponent;
class UTextRenderComponent;
class UMaterialInterface;

// 욕실 거울: 평소엔 숫자가 안 보이다가 Reveal() 호출 시 김이 서리며 숫자가 드러난다
// 숫자가 드러난 뒤에는 F 로 거울을 확대해서 볼 수 있다 (조사 시점). 숫자는 계속 유지된다
UCLASS()
class GSD2_PROJECT_API AFoggedMirror : public AInspectStation
{
	GENERATED_BODY()

public:
	AFoggedMirror();

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual bool CanInspect() const override { return bRevealed; }

	UFUNCTION(BlueprintCallable, Category = "Mirror")
	void Reveal();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mirror")
	TObjectPtr<UStaticMeshComponent> MirrorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mirror")
	TObjectPtr<UTextRenderComponent> CodeText;

	// 거울에 숨겨진 숫자 (휴대폰 보관함 비밀번호와 같게)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mirror")
	FString Code = TEXT("0211");

	// 김 서린 거울 머티리얼 (있으면 Reveal 시 교체, 없으면 그대로)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mirror")
	TObjectPtr<UMaterialInterface> FoggedMaterial;

	// 수도꼭지를 튼 뒤 숫자가 보이기까지 걸리는 시간(초)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mirror")
	float RevealDelay = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mirror")
	FString RevealMessage = TEXT("거울에 김이 서리면서 숫자가 떠올랐다.");

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Mirror")
	bool bRevealed = false;

private:
	void FinishReveal();

	FTimerHandle RevealTimer;
};

// 수도꼭지: F 로 틀면 연결된 거울의 숫자가 드러난다
UCLASS()
class GSD2_PROJECT_API AFaucet : public AInteractableBase
{
	GENERATED_BODY()

public:
	AFaucet();

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetPromptText() const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Faucet")
	TObjectPtr<UStaticMeshComponent> Mesh;

	// 김이 서릴 거울
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faucet")
	TObjectPtr<AFoggedMirror> Mirror;

	// 이 진행 표시가 있어야 틀 수 있음 (기본: 욕조 배수구 조사 후). 비우면 조건 없음
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faucet")
	FName RequiredFlag = TEXT("DrainChecked");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faucet")
	FString TurnOnMessage = TEXT("뜨거운 물을 틀었다. 김이 올라온다...");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faucet")
	FString AlreadyOnMessage = TEXT("물은 계속 흐르고 있다.");

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Faucet")
	bool bTurnedOn = false;
};
