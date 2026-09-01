#include "Player/Exit3PlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Core/Exit3GameState.h"
#include "InputCoreTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/Exit3GameClearWidget.h"
#include "UI/Exit3HUDWidget.h"
#include "UI/Exit3PauseMenuWidget.h"

void AExit3PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UExit3HUDWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToPlayerScreen(0);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s has no HUD Widget Class."), *GetName());
	}

	if (AExit3GameState* State = GetWorld()->GetGameState<AExit3GameState>())
	{
		State->OnGameCleared.AddUniqueDynamic(this, &AExit3PlayerController::HandleGameCleared);
		if (State->IsGameCleared())
		{
			HandleGameCleared();
		}
	}
}

void AExit3PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		FInputKeyBinding& PauseBinding = InputComponent->BindKey(
			EKeys::Escape, IE_Pressed, this, &AExit3PlayerController::TogglePauseMenu);
		PauseBinding.bExecuteWhenPaused = true;
	}
}

void AExit3PlayerController::ShowPauseMenu()
{
	if (GameClearWidgetInstance && GameClearWidgetInstance->IsInViewport())
	{
		return;
	}

	if (!PauseMenuInstance && PauseMenuClass)
	{
		PauseMenuInstance = CreateWidget<UExit3PauseMenuWidget>(this, PauseMenuClass);
	}
	if (!PauseMenuInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s has no valid Pause Menu Class."), *GetName());
		return;
	}

	PauseMenuInstance->AddToViewport(100);

	UGameplayStatics::SetGamePaused(this, true);
	bShowMouseCursor = true;

	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(PauseMenuInstance->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
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

void AExit3PlayerController::TogglePauseMenu()
{
	if (GameClearWidgetInstance && GameClearWidgetInstance->IsInViewport())
	{
		return;
	}

	if (PauseMenuInstance && PauseMenuInstance->IsInViewport())
	{
		HidePauseMenu();
	}
	else
	{
		ShowPauseMenu();
	}
}

void AExit3PlayerController::RestartGame()
{
	UGameplayStatics::SetGamePaused(this, false);
	const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this, true);
	UGameplayStatics::OpenLevel(this, FName(*CurrentLevelName));
}

void AExit3PlayerController::QuitGame()
{
	UKismetSystemLibrary::QuitGame(this, this, EQuitPreference::Quit, false);
}

void AExit3PlayerController::HandleGameCleared()
{
	ShowGameClearScreen();
}

void AExit3PlayerController::ShowGameClearScreen()
{
	if (PauseMenuInstance && PauseMenuInstance->IsInViewport())
	{
		PauseMenuInstance->RemoveFromParent();
	}

	if (!GameClearWidgetInstance && GameClearWidgetClass)
	{
		GameClearWidgetInstance = CreateWidget<UExit3GameClearWidget>(this, GameClearWidgetClass);
	}
	if (!GameClearWidgetInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s has no valid Game Clear Widget Class."), *GetName());
		return;
	}

	GameClearWidgetInstance->AddToViewport(200);
	UGameplayStatics::SetGamePaused(this, true);
	bShowMouseCursor = true;

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(GameClearWidgetInstance->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
}
