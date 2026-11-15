#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableBase.h"
#include "CodeLock.generated.h"

class ACodeLock;
class ADoorBase;
class UChildActorComponent;
class UStaticMeshComponent;
class UTextRenderComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCodeLockOpened);

// 숫자 다이얼 1칸: F 를 누를 때마다 0 → 1 → ... → 9 → 0
UCLASS()
class GSD2_PROJECT_API ACodeDial : public AInteractableBase
{
	GENERATED_BODY()

public:
	ACodeDial();

	virtual void Interact_Implementation(AActor* Interactor) override;

	void SetDigit(int32 NewDigit);
	int32 GetDigit() const { return Digit; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CodeLock")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CodeLock")
	TObjectPtr<UTextRenderComponent> DigitText;

	UPROPERTY(Transient)
	TObjectPtr<ACodeLock> Lock;

private:
	int32 Digit = 0;
};

// 4자리 숫자 자물쇠 (일기장, 휴대폰 보관함 공용)
// 다이얼 4개가 자동으로 붙어 있고, 숫자가 Code 와 같아지면 DoorToUnlock 을 풀고 연다
UCLASS()
class GSD2_PROJECT_API ACodeLock : public AActor
{
	GENERATED_BODY()

public:
	ACodeLock();

	virtual void BeginPlay() override;

	// 다이얼이 바뀔 때 호출됨
	void OnDialChanged();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CodeLock")
	TObjectPtr<UStaticMeshComponent> Plate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CodeLock")
	TArray<TObjectPtr<UChildActorComponent>> DialComponents;

	// 정답 4자리 (예: 일기장 0830, 휴대폰 보관함 = 거울 숫자)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CodeLock")
	FString Code = TEXT("4271");

	// 맞혔을 때 잠금 해제할 문/서랍/보관함
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CodeLock")
	TObjectPtr<ADoorBase> DoorToUnlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CodeLock")
	bool bOpenDoorOnSolved = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CodeLock")
	FString SolvedMessage = TEXT("딸깍. 자물쇠가 풀렸다.");

	// 테스트용 이름표 (화면에 표시, 비우면 안 보임)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	FString DebugLabel = TEXT("숫자 자물쇠 [F: 칸마다 숫자 변경]");

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "CodeLock")
	bool bSolved = false;

	UPROPERTY(BlueprintAssignable, Category = "CodeLock")
	FOnCodeLockOpened OnOpened;

private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<ACodeDial>> Dials;
};
