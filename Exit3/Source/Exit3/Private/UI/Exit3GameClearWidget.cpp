#include "UI/Exit3GameClearWidget.h"

#include "Components/Button.h"
#include "Player/Exit3PlayerController.h"

void UExit3GameClearWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (RestartButton)
	{
		RestartButton->OnClicked.AddUniqueDynamic(this, &UExit3GameClearWidget::HandleRestartClicked);
	}
	if (QuitButton)
	{
		QuitButton->OnClicked.AddUniqueDynamic(this, &UExit3GameClearWidget::HandleQuitClicked);
	}
}

void UExit3GameClearWidget::NativeDestruct()
{
	if (RestartButton)
	{
		RestartButton->OnClicked.RemoveDynamic(this, &UExit3GameClearWidget::HandleRestartClicked);
	}
	if (QuitButton)
	{
		QuitButton->OnClicked.RemoveDynamic(this, &UExit3GameClearWidget::HandleQuitClicked);
	}

	Super::NativeDestruct();
}

void UExit3GameClearWidget::HandleRestartClicked()
{
	if (AExit3PlayerController* PlayerController = GetOwningPlayer<AExit3PlayerController>())
	{
		PlayerController->RestartGame();
	}
}

void UExit3GameClearWidget::HandleQuitClicked()
{
	if (AExit3PlayerController* PlayerController = GetOwningPlayer<AExit3PlayerController>())
	{
		PlayerController->QuitGame();
	}
}
