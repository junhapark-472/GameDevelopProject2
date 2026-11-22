#include "GameHUDWidget.h"
#include "InteractableBase.h"
#include "Components/TextBlock.h"

void UGameHUDWidget::UpdateInteractPrompt(const AInteractableBase* Target)
{
	const FText Prompt = Target ? Target->GetPromptText() : FText::GetEmpty();

	if (!Prompt.IsEmpty())
	{
		CrosshairText->SetColorAndOpacity(FSlateColor(FLinearColor(0.2f, 1.f, 0.3f)));
		InteractPromptText->SetText(FText::Format(FText::FromString(TEXT("[F] {0}")), Prompt));
		InteractPromptText->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		CrosshairText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		InteractPromptText->SetVisibility(ESlateVisibility::Hidden);
	}
}
