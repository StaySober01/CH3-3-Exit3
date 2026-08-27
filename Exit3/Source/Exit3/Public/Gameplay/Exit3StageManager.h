#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Exit3Types.h"
#include "Exit3StageManager.generated.h"

class AExit3AnomalyActor;

UCLASS()
class EXIT3_API AExit3StageManager : public AActor
{
	GENERATED_BODY()

public:
	AExit3StageManager();

	UFUNCTION(BlueprintCallable, Category = "Exit3|Round")
	void PrepareRound(EExit3Stage Stage, EExit3RoundState RoundState);

	UFUNCTION(BlueprintCallable, Category = "Exit3|Round")
	void ResetRound();

	UFUNCTION(BlueprintPure, Category = "Exit3|Round")
	AExit3AnomalyActor* GetActiveAnomaly() const { return ActiveAnomaly; }

private:
	void RefreshAnomalyActors();
	void ActivateRandomAnomaly(EExit3Stage Stage);

	UPROPERTY()
	TArray<TObjectPtr<AExit3AnomalyActor>> AnomalyActors;

	UPROPERTY()
	TObjectPtr<AExit3AnomalyActor> ActiveAnomaly;
};
