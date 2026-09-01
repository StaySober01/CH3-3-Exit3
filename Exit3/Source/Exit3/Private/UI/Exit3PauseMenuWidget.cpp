#include "UI/Exit3PauseMenuWidget.h"

#include "Components/Button.h"
#include "Player/Exit3PlayerController.h"

void UExit3PauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ResumeButton)
	{
		ResumeButton->OnClicked.AddUniqueDynamic(this, &UExit3PauseMenuWidget::HandleResumeClicked);
	}
	if (QuitButton)
	{
		QuitButton->OnClicked.AddUniqueDynamic(this, &UExit3PauseMenuWidget::HandleQuitClicked);
	}
}

void UExit3PauseMenuWidget::NativeDestruct()
{
	if (ResumeButton)
	{
		ResumeButton->OnClicked.RemoveDynamic(this, &UExit3PauseMenuWidget::HandleResumeClicked);
	}
	if (QuitButton)
	{
		QuitButton->OnClicked.RemoveDynamic(this, &UExit3PauseMenuWidget::HandleQuitClicked);
	}

	Super::NativeDestruct();
}

void UExit3PauseMenuWidget::HandleResumeClicked()
{
	if (AExit3PlayerController* PlayerController = GetOwningPlayer<AExit3PlayerController>())
	{
		PlayerController->HidePauseMenu();
	}
}

void UExit3PauseMenuWidget::HandleQuitClicked()
{
	if (AExit3PlayerController* PlayerController = GetOwningPlayer<AExit3PlayerController>())
	{
		PlayerController->QuitGame();
	}
}
