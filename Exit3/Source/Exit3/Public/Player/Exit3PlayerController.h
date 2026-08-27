#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Exit3PlayerController.generated.h"

class UUserWidget;

UCLASS()
class EXIT3_API AExit3PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Exit3|UI")
	void ShowPauseMenu();

	UFUNCTION(BlueprintCallable, Category = "Exit3|UI")
	void HidePauseMenu();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Exit3|UI")
	TSubclassOf<UUserWidget> PauseMenuClass;

private:
	UPROPERTY()
	TObjectPtr<UUserWidget> PauseMenuInstance;
};
