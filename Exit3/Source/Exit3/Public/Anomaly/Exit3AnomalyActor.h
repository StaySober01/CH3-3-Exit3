#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Exit3Types.h"
#include "Exit3AnomalyActor.generated.h"

UCLASS(Abstract, Blueprintable)
class EXIT3_API AExit3AnomalyActor : public AActor
{
	GENERATED_BODY()

public:
	AExit3AnomalyActor();

	UFUNCTION(BlueprintCallable, Category = "Exit3|Anomaly")
	bool ActivateAnomaly();

	UFUNCTION(BlueprintCallable, Category = "Exit3|Anomaly")
	void RestoreAnomaly();

	UFUNCTION(BlueprintPure, Category = "Exit3|Anomaly")
	FName GetAnomalyId() const { return AnomalyId; }

	UFUNCTION(BlueprintPure, Category = "Exit3|Anomaly")
	EExit3Stage GetSupportedStage() const { return SupportedStage; }

	UFUNCTION(BlueprintPure, Category = "Exit3|Anomaly")
	bool IsEnabled() const { return bEnabled; }

	UFUNCTION(BlueprintPure, Category = "Exit3|Anomaly")
	bool IsApplied() const { return bApplied; }

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Exit3|Anomaly")
	void CaptureNormalState();
	virtual void CaptureNormalState_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Exit3|Anomaly")
	void ApplyAnomalyEffect();
	virtual void ApplyAnomalyEffect_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Exit3|Anomaly")
	void RestoreNormalState();
	virtual void RestoreNormalState_Implementation();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Exit3|Anomaly")
	FName AnomalyId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Exit3|Anomaly")
	EExit3Stage SupportedStage = EExit3Stage::Stage1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Exit3|Anomaly")
	bool bEnabled = true;

	UPROPERTY(VisibleInstanceOnly, Category = "Exit3|Anomaly")
	bool bApplied = false;

private:
	bool bNormalStateCaptured = false;
};
