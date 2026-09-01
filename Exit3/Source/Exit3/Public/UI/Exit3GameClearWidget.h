#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Exit3GameClearWidget.generated.h"

class UButton;

UCLASS(Abstract)
class EXIT3_API UExit3GameClearWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void HandleRestartClicked();

	UFUNCTION()
	void HandleQuitClicked();

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> RestartButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> QuitButton;
};
