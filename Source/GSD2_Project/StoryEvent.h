#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StoryEvent.generated.h"

class ADoorBase;
class UStoryWidget;
class UPhoneWidget;

// 대사 한 줄 (말하는 사람 + 내용 + 보여줄 시간)
USTRUCT(BlueprintType)
struct FStoryLine
{
	GENERATED_BODY()

	// 비우면 독백/효과음 설명처럼 이름 없이 표시
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story")
	FText Speaker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story", meta = (MultiLine = true))
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story")
	float Duration = 2.5f;
};

// 이야기 이벤트 (11~15 씬: 범인 공개, 경찰 신고, 엔딩 등)
// - Play() 하면 대사를 차례로 화면 하단에 보여주고 (06강 타이머)
// - 끝나면 진행 표시/목표/힌트를 바꾸고, 문 잠금 해제, 아이템 지급, 다음 이벤트 연결
// - 선택지, 엔딩 화면, 휴대폰 화면도 이 이벤트에서 띄운다
UCLASS()
class GSD2_PROJECT_API AStoryEvent : public AActor
{
	GENERATED_BODY()

public:
	AStoryEvent();

	UFUNCTION(BlueprintCallable, Category = "Story")
	void Play();

	// 위젯 버튼에서 호출
	void OnChoice(int32 Index);
	void OnEndingButton(int32 Index);
	void OnPhoneCalled();

	// 대사 없이 선택지만 다시 보여주기 (배드 엔딩 → 선택지로 돌아가기)
	void ShowChoicesOnly();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story")
	TArray<FStoryLine> Lines;

	// 대사가 나오는 동안 이동/시점 회전 막기
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story")
	bool bLockPlayer = true;

	// 검은 화면 위에 대사 표시 (암전, 엔딩 텍스트)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story")
	bool bBlackScreen = false;

	// ---- 끝났을 때 ----
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story|Result")
	FName FinishFlag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story|Result")
	FText NextObjective;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story|Result")
	FText NextHint;

	// true 면 이후 Q 무전기는 잡음만 (준하 님 GameInstance 기능 사용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story|Result")
	bool bRadioNoiseOnly = false;

	// 인벤토리에 넣어줄 아이템
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story|Result")
	TArray<FName> GiveItems;

	// 숨길 맵 액터 이름 (예: 부서진 CCTV)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story|Result")
	TArray<FName> HideActorNames;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story|Result")
	TArray<TObjectPtr<ADoorBase>> UnlockDoors;

	// 이어서 재생할 이벤트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story|Result")
	TObjectPtr<AStoryEvent> NextEvent;

	// ---- 휴대폰 화면 ----
	// true 면 대사가 끝난 뒤 휴대폰 화면을 띄우고, 112 로 전화하면 PhoneCallEvent 재생
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story|Phone")
	bool bOpenPhone = false;

	// true 면 보조배터리(Battery)가 있어야 휴대폰이 켜짐
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story|Phone")
	bool bRequireBattery = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story|Phone")
	TObjectPtr<AStoryEvent> PhoneCallEvent;

	// ---- 선택지 ----
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story|Choice")
	FText ChoiceTextA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story|Choice")
	FText ChoiceTextB;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story|Choice")
	TObjectPtr<AStoryEvent> ChoiceEventA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story|Choice")
	TObjectPtr<AStoryEvent> ChoiceEventB;

	// ---- 엔딩 ----
	// 비어 있지 않으면 대사가 끝난 뒤 엔딩 화면 (예: BAD ENDING)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story|Ending")
	FText EndingTitle;

	// 있으면 [선택지로 돌아가기] 버튼 → 이 이벤트의 선택지를 다시 보여줌, 없으면 [처음부터 다시하기]
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story|Ending")
	TObjectPtr<AStoryEvent> RetryEvent;

private:
	void ShowNextLine();
	void Finish();
	void SetPlayerLocked(bool bLocked, bool bShowCursor);
	UStoryWidget* GetWidget();
	void CloseWidget();

	// 버튼 처리는 클릭이 끝난 다음 프레임에 한다 (클릭 도중 위젯을 지우지 않기 위해)
	void HandleChoice();
	void HandleEndingButton();
	int32 PendingIndex = 0;

	UPROPERTY(Transient)
	TObjectPtr<UStoryWidget> StoryWidget;

	UPROPERTY(Transient)
	TObjectPtr<UPhoneWidget> PhoneWidget;

	int32 LineIndex = 0;
	FTimerHandle LineTimer;
};
