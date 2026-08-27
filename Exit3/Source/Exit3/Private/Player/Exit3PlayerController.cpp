#include "Player/Exit3PlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void AExit3PlayerController::ShowPauseMenu()
{
	if (!PauseMenuInstance && PauseMenuClass)
	{
		PauseMenuInstance = CreateWidget<UUserWidget>(this, PauseMenuClass);
	}
	if (PauseMenuInstance)
	{
		PauseMenuInstance->AddToViewport();
	}

	UGameplayStatics::SetGamePaused(this, true);
	bShowMouseCursor = true;
	SetInputMode(FInputModeUIOnly());
}

void AExit3PlayerController::HidePauseMenu()
{
	if (PauseMenuInstance)
	{
		PauseMenuInstance->RemoveFromParent();
	}

	UGameplayStatics::SetGamePaused(this, false);
	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());
}
