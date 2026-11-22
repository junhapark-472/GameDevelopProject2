#include "CodeLockWidget.h"
#include "CodeLock.h"
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

void UCodeLockWidget::Setup(ACodeLock* InLock)
{
	Lock = InLock;
	if (!Lock || !WidgetTree) return;

	// 화면 전체 캔버스 → 화면 아래쪽 가운데에 반투명 상자 (위쪽에는 자물쇠 모델이 보이도록)
	UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	WidgetTree->RootWidget = Canvas;

	UBorder* Panel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	Panel->SetBrushColor(FLinearColor(0.f, 0.f, 0.f, 0.65f));
	Panel->SetPadding(FMargin(40.f, 24.f));
	if (UCanvasPanelSlot* PanelSlot = Canvas->AddChildToCanvas(Panel))
	{
		PanelSlot->SetAnchors(FAnchors(0.5f, 0.78f));
		PanelSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		PanelSlot->SetAutoSize(true);
	}

	UVerticalBox* Column = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	Panel->SetContent(Column);

	// 자릿수만큼 [▲ / 숫자 / ▼] 를 가로로 나열
	UHorizontalBox* DialRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	Column->AddChildToVerticalBox(DialRow);

	for (int32 i = 0; i < Lock->GetNumDigits(); ++i)
	{
		UVerticalBox* Dial = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
		if (UHorizontalBoxSlot* DialSlot = DialRow->AddChildToHorizontalBox(Dial))
		{
			DialSlot->SetPadding(FMargin(14.f, 0.f));
		}

		UButton* Up = MakeButton(TEXT("▲"), 26.f);
		Up->OnClicked.AddDynamic(this, &UCodeLockWidget::OnUpClicked);

		UTextBlock* Digit = MakeText(TEXT("0"), 64.f);

		UButton* Down = MakeButton(TEXT("▼"), 26.f);
		Down->OnClicked.AddDynamic(this, &UCodeLockWidget::OnDownClicked);

		Dial->AddChildToVerticalBox(Up);
		if (UVerticalBoxSlot* DigitSlot = Dial->AddChildToVerticalBox(Digit))
		{
			DigitSlot->SetHorizontalAlignment(HAlign_Center);
			DigitSlot->SetPadding(FMargin(0.f, 4.f));
		}
		Dial->AddChildToVerticalBox(Down);

		UpButtons.Add(Up);
		DigitTexts.Add(Digit);
		DownButtons.Add(Down);
	}

	UButton* Confirm = MakeButton(TEXT("확인"), 26.f);
	Confirm->OnClicked.AddDynamic(this, &UCodeLockWidget::OnConfirmClicked);
	if (UVerticalBoxSlot* ConfirmSlot = Column->AddChildToVerticalBox(Confirm))
	{
		ConfirmSlot->SetPadding(FMargin(10.f, 16.f, 10.f, 0.f));
	}

	UTextBlock* Help = MakeText(TEXT("마우스 오른쪽 클릭 : 나가기"), 16.f);
	Help->SetColorAndOpacity(FSlateColor(FLinearColor(0.7f, 0.7f, 0.7f)));
	if (UVerticalBoxSlot* HelpSlot = Column->AddChildToVerticalBox(Help))
	{
		HelpSlot->SetHorizontalAlignment(HAlign_Center);
		HelpSlot->SetPadding(FMargin(0.f, 10.f, 0.f, 0.f));
	}

	RefreshDigits();
}

void UCodeLockWidget::RefreshDigits()
{
	if (!Lock) return;

	for (int32 i = 0; i < DigitTexts.Num(); ++i)
	{
		DigitTexts[i]->SetText(FText::AsNumber(Lock->GetDigit(i)));
	}
}

void UCodeLockWidget::OnUpClicked()
{
	// 어느 자리의 버튼인지: 지금 마우스가 올라가 있는 버튼을 찾는다
	for (int32 i = 0; i < UpButtons.Num(); ++i)
	{
		if (UpButtons[i]->IsHovered())
		{
			Lock->ChangeDigit(i, 1);
		}
	}
	RefreshDigits();
}

void UCodeLockWidget::OnDownClicked()
{
	for (int32 i = 0; i < DownButtons.Num(); ++i)
	{
		if (DownButtons[i]->IsHovered())
		{
			Lock->ChangeDigit(i, -1);
		}
	}
	RefreshDigits();
}

void UCodeLockWidget::OnConfirmClicked()
{
	if (Lock)
	{
		Lock->TryOpen();
	}
}

UButton* UCodeLockWidget::MakeButton(const FString& Label, float FontSize)
{
	UButton* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	Button->SetBackgroundColor(FLinearColor(0.25f, 0.25f, 0.25f, 1.f));
	Button->AddChild(MakeText(Label, FontSize));
	return Button;
}

UTextBlock* UCodeLockWidget::MakeText(const FString& Text, float FontSize)
{
	UTextBlock* Block = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	Block->SetText(FText::FromString(Text));

	FSlateFontInfo Font = Block->GetFont();
	Font.Size = FontSize;
	Block->SetFont(Font);
	Block->SetJustification(ETextJustify::Center);
	return Block;
}
