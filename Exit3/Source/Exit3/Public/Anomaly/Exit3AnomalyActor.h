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

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Exit3|Anomaly")
	void ApplyAnomaly();
	virtual void ApplyAnomaly_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Exit3|Anomaly")
	void ResetAnomaly();
	virtual void ResetAnomaly_Implementation();

	UFUNCTION(BlueprintPure, Category = "Exit3|Anomaly")
	EExit3Stage GetSupportedStage() const { return SupportedStage; }

	UFUNCTION(BlueprintPure, Category = "Exit3|Anomaly")
	bool IsApplied() const { return bApplied; }

protected:
	UPROPERTY(EditAnywhere, Category = "Exit3|Anomaly")
	FName AnomalyId = NAME_None;

	UPROPERTY(EditAnywhere, Category = "Exit3|Anomaly")
	EExit3Stage SupportedStage = EExit3Stage::Stage1;

	UPROPERTY(VisibleInstanceOnly, Category = "Exit3|Anomaly")
	bool bApplied = false;
};
