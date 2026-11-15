#include "GameHUDWidget.h"
#include "InteractableBase.h"
#include "Components/TextBlock.h"

void UGameHUDWidget::UpdateInteractPrompt(const AInteractableBase* Target)
{
	if (Target)
	{
		CrosshairText->SetColorAndOpacity(FSlateColor(FLinearColor(0.2f, 1.f, 0.3f)));
		InteractPromptText->SetText(FText::Format(FText::FromString(TEXT("F - {0}")), Target->GetPromptText()));
		InteractPromptText->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		CrosshairText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		InteractPromptText->SetVisibility(ESlateVisibility::Hidden);
	}
}
