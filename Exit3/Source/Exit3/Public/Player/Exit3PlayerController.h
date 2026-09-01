#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Exit3PlayerController.generated.h"

class UExit3GameClearWidget;
class UExit3HUDWidget;
class UExit3PauseMenuWidget;

UCLASS()
class EXIT3_API AExit3PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Exit3|UI")
	void ShowPauseMenu();

	UFUNCTION(BlueprintCallable, Category = "Exit3|UI")
	void HidePauseMenu();

	UFUNCTION(BlueprintCallable, Category = "Exit3|UI")
	void TogglePauseMenu();

	UFUNCTION(BlueprintCallable, Category = "Exit3|Game")
	void RestartGame();

	UFUNCTION(BlueprintCallable, Category = "Exit3|Game")
	void QuitGame();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly, Category = "Exit3|UI")
	TSubclassOf<UExit3HUDWidget> HUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Exit3|UI")
	TSubclassOf<UExit3PauseMenuWidget> PauseMenuClass;

	UPROPERTY(EditDefaultsOnly, Category = "Exit3|UI")
	TSubclassOf<UExit3GameClearWidget> GameClearWidgetClass;

private:
	UFUNCTION()
	void HandleGameCleared();

	void ShowGameClearScreen();

	UPROPERTY()
	TObjectPtr<UExit3HUDWidget> HUDWidgetInstance;

	UPROPERTY()
	TObjectPtr<UExit3PauseMenuWidget> PauseMenuInstance;

	UPROPERTY()
	TObjectPtr<UExit3GameClearWidget> GameClearWidgetInstance;
};
