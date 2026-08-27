#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/Exit3Types.h"
#include "Exit3HUDWidget.generated.h"

class UTextBlock;

UCLASS(Abstract)
class EXIT3_API UExit3HUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void HandleStageChanged(EExit3Stage NewStage);

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> StageText;
};
