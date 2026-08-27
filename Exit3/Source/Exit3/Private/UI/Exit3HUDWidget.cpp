#include "UI/Exit3HUDWidget.h"

#include "Components/TextBlock.h"
#include "Core/Exit3GameState.h"

void UExit3HUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (AExit3GameState* State = GetWorld()->GetGameState<AExit3GameState>())
	{
		State->OnStageChanged.AddUniqueDynamic(this, &UExit3HUDWidget::HandleStageChanged);
		HandleStageChanged(State->GetCurrentStage());
	}
}

void UExit3HUDWidget::HandleStageChanged(const EExit3Stage NewStage)
{
	if (StageText)
	{
		StageText->SetText(FText::Format(NSLOCTEXT("Exit3", "StageFormat", "STAGE {0} / 3"),
			FText::AsNumber(static_cast<uint8>(NewStage))));
	}
}
