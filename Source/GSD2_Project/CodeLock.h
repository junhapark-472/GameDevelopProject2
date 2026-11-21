#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableBase.h"
#include "CodeLock.generated.h"

class ACodeLock;
class ADoorBase;
class UStaticMeshComponent;
class UTextRenderComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCodeLockOpened);

// 숫자 다이얼 1칸: F 를 누를 때마다 0 → 1 → ... → 9 → 0
// CodeLock 이 게임 시작 시 자동으로 생성한다 (직접 배치하지 않음)
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
// 게임 시작 시 다이얼을 Code 자릿수만큼 앞면(+X)에 생성하고, 숫자가 Code 와 같아지면 DoorToUnlock 을 푼다
UCLASS()
class GSD2_PROJECT_API ACodeLock : public AActor
{
	GENERATED_BODY()

public:
	ACodeLock();

	virtual void BeginPlay() override;

	// 다이얼 숫자가 바뀔 때마다 다이얼이 호출
	void OnDialChanged();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CodeLock")
	TObjectPtr<UStaticMeshComponent> Plate;

	// 정답 (예: 일기장 0830, 휴대폰 보관함 = 거울 숫자). 자릿수만큼 다이얼이 생긴다
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CodeLock")
	FString Code = TEXT("4271");

	// 맞혔을 때 잠금 해제할 문/서랍/보관함
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CodeLock")
	TObjectPtr<ADoorBase> DoorToUnlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CodeLock")
	bool bOpenDoorOnSolved = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CodeLock")
	FString SolvedMessage = TEXT("딸깍. 자물쇠가 풀렸다.");

	// 다이얼 사이 간격
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CodeLock")
	float DialSpacing = 14.f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "CodeLock")
	bool bSolved = false;

	UPROPERTY(BlueprintAssignable, Category = "CodeLock")
	FOnCodeLockOpened OnOpened;

private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<ACodeDial>> Dials;
};
