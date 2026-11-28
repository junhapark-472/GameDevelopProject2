#include "StoryEvent.h"
#include "StoryWidget.h"
#include "PhoneWidget.h"
#include "DoorBase.h"
#include "GSD2FunctionLibrary.h"
#include "GSD2GameInstance.h"
#include "InventoryComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"

AStoryEvent::AStoryEvent()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void AStoryEvent::Play()
{
	LineIndex = 0;

	if (Lines.Num() > 0)
	{
		SetPlayerLocked(bLockPlayer, false);
		GetWidget()->SetBlack(bBlackScreen);
	}
	ShowNextLine();
}

void AStoryEvent::ShowNextLine()
{
	if (!Lines.IsValidIndex(LineIndex))
	{
		Finish();
		return;
	}

	const FStoryLine& Line = Lines[LineIndex];
	GetWidget()->ShowLine(Line.Speaker, Line.Text);
	++LineIndex;

	// 이 줄의 시간이 지나면 다음 줄 (06강 타이머)
	GetWorldTimerManager().SetTimer(LineTimer, this, &AStoryEvent::ShowNextLine, FMath::Max(Line.Duration, 0.1f), false);
}

void AStoryEvent::Finish()
{
	// 진행 표시 / 목표 / 힌트
	UGSD2FunctionLibrary::SetProgressFlag(this, FinishFlag);
	UGSD2FunctionLibrary::UpdateObjectiveAndHint(this, NextObjective, NextHint);

	if (bRadioNoiseOnly)
	{
		if (UGSD2GameInstance* GameInstance = Cast<UGSD2GameInstance>(UGameplayStatics::GetGameInstance(this)))
		{
			GameInstance->SetRadioNoiseOnly(true);
		}
	}

	// 아이템 지급
	if (GiveItems.Num() > 0)
	{
		APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0);
		UInventoryComponent* Inventory = Pawn ? Pawn->FindComponentByClass<UInventoryComponent>() : nullptr;
		for (const FName& ItemID : GiveItems)
		{
			if (Inventory)
			{
				Inventory->AddItem(ItemID);
			}
		}
	}

	// 맵 액터 숨기기 (이름으로 찾기)
	if (HideActorNames.Num() > 0)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(this, AActor::StaticClass(), FoundActors);
		for (AActor* Actor : FoundActors)
		{
			if (Actor && HideActorNames.Contains(Actor->GetFName()))
			{
				Actor->SetActorHiddenInGame(true);
				Actor->SetActorEnableCollision(false);
			}
		}
	}

	for (ADoorBase* Door : UnlockDoors)
	{
		if (Door)
		{
			Door->Unlock();
		}
	}

	// 엔딩 화면
	if (!EndingTitle.IsEmpty())
	{
		SetPlayerLocked(true, true);
		GetWidget()->ShowEnding(EndingTitle, RetryEvent != nullptr);
		return;
	}

	// 선택지
	if (ChoiceEventA || ChoiceEventB)
	{
		ShowChoicesOnly();
		return;
	}

	// 휴대폰 화면
	if (bOpenPhone)
	{
		CloseWidget();
		SetPlayerLocked(true, true);

		APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0);
		UInventoryComponent* Inventory = Pawn ? Pawn->FindComponentByClass<UInventoryComponent>() : nullptr;
		const bool bHasBattery = !bRequireBattery || (Inventory && Inventory->HasItem(TEXT("Battery")));

		PhoneWidget = CreateWidget<UPhoneWidget>(UGameplayStatics::GetPlayerController(this, 0), UPhoneWidget::StaticClass());
		if (PhoneWidget)
		{
			PhoneWidget->Setup(this, bHasBattery);
			PhoneWidget->AddToViewport(10);
		}
		return;
	}

	CloseWidget();
	SetPlayerLocked(false, false);

	if (NextEvent)
	{
		NextEvent->Play();
	}
}

void AStoryEvent::ShowChoicesOnly()
{
	SetPlayerLocked(true, true);
	GetWidget()->SetBlack(false);
	GetWidget()->ShowChoices(ChoiceTextA, ChoiceTextB);
}

void AStoryEvent::OnChoice(int32 Index)
{
	PendingIndex = Index;
	GetWorldTimerManager().SetTimerForNextTick(this, &AStoryEvent::HandleChoice);
}

void AStoryEvent::HandleChoice()
{
	const int32 Index = PendingIndex;
	CloseWidget();
	SetPlayerLocked(false, false);

	AStoryEvent* Chosen = (Index == 0) ? ChoiceEventA.Get() : ChoiceEventB.Get();
	if (Chosen)
	{
		Chosen->Play();
	}
}

void AStoryEvent::OnEndingButton(int32 Index)
{
	PendingIndex = Index;
	GetWorldTimerManager().SetTimerForNextTick(this, &AStoryEvent::HandleEndingButton);
}

void AStoryEvent::HandleEndingButton()
{
	const int32 Index = PendingIndex;
	if (Index == 0)
	{
		if (RetryEvent)
		{
			// 선택지로 돌아가기
			CloseWidget();
			RetryEvent->ShowChoicesOnly();
		}
		else
		{
			// 처음부터 다시하기 (메인 메뉴가 생기면 메인 메뉴로 바꿀 것)
			if (UGSD2GameInstance* GameInstance = Cast<UGSD2GameInstance>(UGameplayStatics::GetGameInstance(this)))
			{
				GameInstance->ResetProgress();
			}
			UGameplayStatics::OpenLevel(this, FName(*UGameplayStatics::GetCurrentLevelName(this)));
		}
		return;
	}

	// 게임 종료
	UKismetSystemLibrary::QuitGame(this, UGameplayStatics::GetPlayerController(this, 0), EQuitPreference::Quit, false);
}

void AStoryEvent::OnPhoneCalled()
{
	if (PhoneWidget)
	{
		PhoneWidget->RemoveFromParent();
		PhoneWidget = nullptr;
	}
	SetPlayerLocked(false, false);

	if (PhoneCallEvent)
	{
		PhoneCallEvent->Play();
	}
}

void AStoryEvent::SetPlayerLocked(bool bLocked, bool bShowCursor)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;

	PC->SetIgnoreMoveInput(bLocked);
	PC->SetIgnoreLookInput(bLocked);
	PC->bShowMouseCursor = bShowCursor;

	if (bShowCursor)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(false);
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
	}
	else
	{
		PC->SetInputMode(FInputModeGameOnly());
	}

	if (!bLocked)
	{
		// SetIgnore... 는 누적되므로 완전히 풀어 준다
		PC->ResetIgnoreMoveInput();
		PC->ResetIgnoreLookInput();
	}
}

UStoryWidget* AStoryEvent::GetWidget()
{
	if (!StoryWidget)
	{
		StoryWidget = CreateWidget<UStoryWidget>(UGameplayStatics::GetPlayerController(this, 0), UStoryWidget::StaticClass());
		StoryWidget->Setup(this);
		StoryWidget->AddToViewport(10);
	}
	return StoryWidget;
}

void AStoryEvent::CloseWidget()
{
	if (StoryWidget)
	{
		StoryWidget->RemoveFromParent();
		StoryWidget = nullptr;
	}
}
