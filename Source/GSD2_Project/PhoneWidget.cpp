#include "PhoneWidget.h"
#include "StoryEvent.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "TimerManager.h"

void UPhoneWidget::Setup(AStoryEvent* InOwnerEvent, bool bInHasBattery)
{
	OwnerEvent = InOwnerEvent;
	bHasBattery = bInHasBattery;
	if (!WidgetTree) return;

	UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	WidgetTree->RootWidget = Canvas;

	// 휴대폰 몸체 (화면 가운데)
	UBorder* Body = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	Body->SetBrushColor(FLinearColor(0.08f, 0.08f, 0.09f, 1.f));
	Body->SetPadding(FMargin(18.f, 24.f));
	if (UCanvasPanelSlot* BodySlot = Canvas->AddChildToCanvas(Body))
	{
		BodySlot->SetAnchors(FAnchors(0.5f, 0.5f));
		BodySlot->SetAlignment(FVector2D(0.5f, 0.5f));
		BodySlot->SetSize(FVector2D(340.f, 600.f));
	}

	UVerticalBox* Column = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	Body->SetContent(Column);

	// 휴대폰 화면 (검은 판 + 글씨)
	UBorder* Screen = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	Screen->SetBrushColor(FLinearColor::Black);
	Screen->SetPadding(FMargin(10.f, 40.f));
	ScreenText = MakeText(TEXT(""), 26.f);
	ScreenText->SetAutoWrapText(true);
	Screen->SetContent(ScreenText);
	if (UVerticalBoxSlot* ScreenSlot = Column->AddChildToVerticalBox(Screen))
	{
		ScreenSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	}

	// 아래 버튼 영역 (상태마다 버튼을 다시 만든다)
	ButtonArea = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	if (UVerticalBoxSlot* AreaSlot = Column->AddChildToVerticalBox(ButtonArea))
	{
		AreaSlot->SetPadding(FMargin(0.f, 12.f, 0.f, 0.f));
	}

	// 처음: 꺼져 있는 휴대폰
	AddButton(TEXT("전원 버튼"))->OnClicked.AddDynamic(this, &UPhoneWidget::OnPowerClicked);
}

void UPhoneWidget::OnPowerClicked()
{
	ClearButtons();
	ScreenText->SetText(FText::FromString(TEXT("...\n\n(화면이 켜지지 않는다)\n배터리가 없잖아...")));

	if (bHasBattery)
	{
		AddButton(TEXT("보조배터리 연결"))->OnClicked.AddDynamic(this, &UPhoneWidget::OnBatteryClicked);
	}
	else
	{
		ScreenText->SetText(FText::FromString(TEXT("(화면이 켜지지 않는다)\n\n보조배터리가 있어야 할 것 같다.")));
	}
}

void UPhoneWidget::OnBatteryClicked()
{
	ClearButtons();
	ScreenText->SetText(FText::FromString(TEXT("충전 중...")));

	// 잠시 후 잠금 화면 (06강 타이머, 긴 대기는 두지 않음)
	GetWorld()->GetTimerManager().SetTimer(ChargeTimer, this, &UPhoneWidget::ShowLockScreen, 1.5f, false);
}

void UPhoneWidget::ShowLockScreen()
{
	ClearButtons();
	ScreenText->SetText(FText::FromString(TEXT("03:24\n\n잠금 화면\n\n됐다...!")));
	AddButton(TEXT("긴급전화"))->OnClicked.AddDynamic(this, &UPhoneWidget::OnEmergencyClicked);
}

void UPhoneWidget::OnEmergencyClicked()
{
	ClearButtons();
	DialNumber.Empty();
	ScreenText->SetText(FText::FromString(TEXT("긴급전화\n\n_")));

	// 숫자 키패드: 1 2 3 / 4 5 6 / 7 8 9 / 지우기 0 통화
	DigitButtons.SetNum(10);
	const int32 Layout[3][3] = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9} };
	for (int32 Row = 0; Row < 4; ++Row)
	{
		UHorizontalBox* RowBox = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
		if (UVerticalBoxSlot* RowSlot = ButtonArea->AddChildToVerticalBox(RowBox))
		{
			RowSlot->SetPadding(FMargin(0.f, 3.f));
		}

		for (int32 Col = 0; Col < 3; ++Col)
		{
			UButton* Button = nullptr;
			if (Row < 3)
			{
				const int32 Digit = Layout[Row][Col];
				Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
				Button->AddChild(MakeText(FString::FromInt(Digit), 22.f));
				Button->OnClicked.AddDynamic(this, &UPhoneWidget::OnDigitClicked);
				DigitButtons[Digit] = Button;
			}
			else
			{
				Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
				if (Col == 0)
				{
					Button->AddChild(MakeText(TEXT("지우기"), 16.f));
					Button->OnClicked.AddDynamic(this, &UPhoneWidget::OnDeleteClicked);
				}
				else if (Col == 1)
				{
					Button->AddChild(MakeText(TEXT("0"), 22.f));
					Button->OnClicked.AddDynamic(this, &UPhoneWidget::OnDigitClicked);
					DigitButtons[0] = Button;
				}
				else
				{
					Button->AddChild(MakeText(TEXT("통화"), 16.f));
					Button->SetBackgroundColor(FLinearColor(0.2f, 0.8f, 0.3f, 1.f));
					Button->OnClicked.AddDynamic(this, &UPhoneWidget::OnCallClicked);
				}
			}

			if (UHorizontalBoxSlot* ButtonSlot = RowBox->AddChildToHorizontalBox(Button))
			{
				ButtonSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
				ButtonSlot->SetPadding(FMargin(3.f, 0.f));
			}
		}
	}
}

void UPhoneWidget::OnDigitClicked()
{
	// 어느 숫자인지: 마우스가 올라가 있는 버튼을 찾는다
	for (int32 Digit = 0; Digit < DigitButtons.Num(); ++Digit)
	{
		if (DigitButtons[Digit] && DigitButtons[Digit]->IsHovered() && DialNumber.Len() < 12)
		{
			DialNumber.AppendInt(Digit);
		}
	}
	ScreenText->SetText(FText::FromString(FString::Printf(TEXT("긴급전화\n\n%s"), *DialNumber)));
}

void UPhoneWidget::OnDeleteClicked()
{
	if (!DialNumber.IsEmpty())
	{
		DialNumber.LeftChopInline(1);
	}
	ScreenText->SetText(FText::FromString(FString::Printf(TEXT("긴급전화\n\n%s"), *DialNumber)));
}

void UPhoneWidget::OnCallClicked()
{
	if (DialNumber != TEXT("112"))
	{
		ScreenText->SetText(FText::FromString(FString::Printf(TEXT("%s\n\n연결할 수 없는 번호입니다."), *DialNumber)));
		DialNumber.Empty();
		return;
	}

	if (OwnerEvent)
	{
		OwnerEvent->OnPhoneCalled();
	}
}

void UPhoneWidget::ClearButtons()
{
	ButtonArea->ClearChildren();
	DigitButtons.Reset();
}

UButton* UPhoneWidget::AddButton(const FString& Label)
{
	UButton* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	Button->SetBackgroundColor(FLinearColor(0.25f, 0.25f, 0.25f, 1.f));
	Button->AddChild(MakeText(Label, 20.f));
	if (UVerticalBoxSlot* ButtonSlot = ButtonArea->AddChildToVerticalBox(Button))
	{
		ButtonSlot->SetPadding(FMargin(0.f, 4.f));
	}
	return Button;
}

UTextBlock* UPhoneWidget::MakeText(const FString& Text, float FontSize)
{
	UTextBlock* Block = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	Block->SetText(FText::FromString(Text));

	FSlateFontInfo Font = Block->GetFont();
	Font.Size = FontSize;
	Block->SetFont(Font);
	Block->SetJustification(ETextJustify::Center);
	return Block;
}
