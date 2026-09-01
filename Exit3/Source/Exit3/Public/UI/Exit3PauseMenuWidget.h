#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Exit3PauseMenuWidget.generated.h"

class UButton;

UCLASS(Abstract)
class EXIT3_API UExit3PauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void HandleResumeClicked();

	UFUNCTION()
	void HandleQuitClicked();

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> ResumeButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> QuitButton;
};
