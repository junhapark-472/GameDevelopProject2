#include "StoryWidget.h"
#include "StoryEvent.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

void UStoryWidget::Setup(AStoryEvent* InOwnerEvent)
{
	OwnerEvent = InOwnerEvent;
	if (!WidgetTree) return;

	UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	WidgetTree->RootWidget = Canvas;

	// 1) 화면 전체 검은 판 (암전)
	BlackScreen = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	BlackScreen->SetBrushColor(FLinearColor::Black);
	if (UCanvasPanelSlot* BlackSlot = Canvas->AddChildToCanvas(BlackScreen))
	{
		BlackSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
		BlackSlot->SetOffsets(FMargin(0.f));
	}
	BlackScreen->SetVisibility(ESlateVisibility::Hidden);

	// 2) 화면 하단 대사창 (이름 + 대사)
	DialogueBox = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	DialogueBox->SetBrushColor(FLinearColor(0.f, 0.f, 0.f, 0.7f));
	DialogueBox->SetPadding(FMargin(30.f, 16.f));
	if (UCanvasPanelSlot* BoxSlot = Canvas->AddChildToCanvas(DialogueBox))
	{
		BoxSlot->SetAnchors(FAnchors(0.15f, 0.78f, 0.85f, 0.95f));
		BoxSlot->SetOffsets(FMargin(0.f));
	}

	DialogueBox->SetVisibility(ESlateVisibility::Hidden);

	UVerticalBox* DialogueColumn = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	DialogueBox->SetContent(DialogueColumn);

	SpeakerText = MakeText(FText::GetEmpty(), 18.f);
	SpeakerText->SetColorAndOpacity(FSlateColor(FLinearColor(1.f, 0.85f, 0.4f)));
	SpeakerText->SetJustification(ETextJustify::Left);
	DialogueColumn->AddChildToVerticalBox(SpeakerText);

	LineText = MakeText(FText::GetEmpty(), 22.f);
	LineText->SetJustification(ETextJustify::Left);
	LineText->SetAutoWrapText(true);
	if (UVerticalBoxSlot* LineSlot = DialogueColumn->AddChildToVerticalBox(LineText))
	{
		LineSlot->SetPadding(FMargin(0.f, 6.f, 0.f, 0.f));
	}

	// 3) 화면 가운데 선택지
	ChoiceBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	if (UCanvasPanelSlot* ChoiceSlot = Canvas->AddChildToCanvas(ChoiceBox))
	{
		ChoiceSlot->SetAnchors(FAnchors(0.5f, 0.5f));
		ChoiceSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		ChoiceSlot->SetAutoSize(true);
	}
	ChoiceBox->SetVisibility(ESlateVisibility::Collapsed);

	// 4) 화면 가운데 엔딩 제목 + 버튼
	EndingBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	if (UCanvasPanelSlot* EndingSlot = Canvas->AddChildToCanvas(EndingBox))
	{
		EndingSlot->SetAnchors(FAnchors(0.5f, 0.5f));
		EndingSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		EndingSlot->SetAutoSize(true);
	}
	EndingBox->SetVisibility(ESlateVisibility::Collapsed);
}

void UStoryWidget::ShowLine(const FText& Speaker, const FText& Text)
{
	DialogueBox->SetVisibility(ESlateVisibility::HitTestInvisible);
	SpeakerText->SetText(Speaker);
	SpeakerText->SetVisibility(Speaker.IsEmpty() ? ESlateVisibility::Collapsed : ESlateVisibility::HitTestInvisible);
	LineText->SetText(Text);
}

void UStoryWidget::SetBlack(bool bBlack)
{
	BlackScreen->SetVisibility(bBlack ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
}

void UStoryWidget::ShowChoices(const FText& ChoiceA, const FText& ChoiceB)
{
	ChoiceBox->ClearChildren();

	UButton* ButtonA = MakeButton(ChoiceA, 26.f);
	ButtonA->OnClicked.AddDynamic(this, &UStoryWidget::OnChoiceAClicked);
	UButton* ButtonB = MakeButton(ChoiceB, 26.f);
	ButtonB->OnClicked.AddDynamic(this, &UStoryWidget::OnChoiceBClicked);

	ChoiceBox->AddChildToVerticalBox(ButtonA);
	if (UVerticalBoxSlot* SlotB = ChoiceBox->AddChildToVerticalBox(ButtonB))
	{
		SlotB->SetPadding(FMargin(0.f, 16.f, 0.f, 0.f));
	}
	ChoiceBox->SetVisibility(ESlateVisibility::Visible);
}

void UStoryWidget::ShowEnding(const FText& Title, bool bCanRetry)
{
	SetBlack(true);
	DialogueBox->SetVisibility(ESlateVisibility::Hidden);
	ChoiceBox->SetVisibility(ESlateVisibility::Collapsed);
	EndingBox->ClearChildren();

	UTextBlock* TitleText = MakeText(Title, 56.f);
	EndingBox->AddChildToVerticalBox(TitleText);

	UButton* FirstButton = MakeButton(FText::FromString(bCanRetry ? TEXT("선택지로 돌아가기") : TEXT("처음부터 다시하기")), 20.f);
	FirstButton->OnClicked.AddDynamic(this, &UStoryWidget::OnEndingFirstClicked);
	if (UVerticalBoxSlot* FirstSlot = EndingBox->AddChildToVerticalBox(FirstButton))
	{
		FirstSlot->SetPadding(FMargin(0.f, 30.f, 0.f, 0.f));
	}

	UButton* QuitButton = MakeButton(FText::FromString(TEXT("게임 종료")), 20.f);
	QuitButton->OnClicked.AddDynamic(this, &UStoryWidget::OnEndingQuitClicked);
	if (UVerticalBoxSlot* QuitSlot = EndingBox->AddChildToVerticalBox(QuitButton))
	{
		QuitSlot->SetPadding(FMargin(0.f, 10.f, 0.f, 0.f));
	}

	EndingBox->SetVisibility(ESlateVisibility::Visible);
}

void UStoryWidget::OnChoiceAClicked()
{
	if (OwnerEvent) OwnerEvent->OnChoice(0);
}

void UStoryWidget::OnChoiceBClicked()
{
	if (OwnerEvent) OwnerEvent->OnChoice(1);
}

void UStoryWidget::OnEndingFirstClicked()
{
	if (OwnerEvent) OwnerEvent->OnEndingButton(0);
}

void UStoryWidget::OnEndingQuitClicked()
{
	if (OwnerEvent) OwnerEvent->OnEndingButton(1);
}

UButton* UStoryWidget::MakeButton(const FText& Label, float FontSize)
{
	UButton* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	Button->SetBackgroundColor(FLinearColor(0.2f, 0.2f, 0.2f, 1.f));
	UTextBlock* LabelText = MakeText(Label, FontSize);
	Button->AddChild(LabelText);
	return Button;
}

UTextBlock* UStoryWidget::MakeText(const FText& Text, float FontSize)
{
	UTextBlock* Block = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	Block->SetText(Text);

	FSlateFontInfo Font = Block->GetFont();
	Font.Size = FontSize;
	Block->SetFont(Font);
	Block->SetJustification(ETextJustify::Center);
	return Block;
}
