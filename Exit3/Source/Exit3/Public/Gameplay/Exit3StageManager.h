#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Exit3Types.h"
#include "Exit3StageManager.generated.h"

class AExit3AnomalyActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FExit3AnomalyActivated,
	FName, AnomalyId,
	EExit3Stage, Stage,
	AExit3AnomalyActor*, AnomalyActor);

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

	UFUNCTION(BlueprintPure, Category = "Exit3|Round")
	FName GetActiveAnomalyId() const;

	UFUNCTION(BlueprintPure, Category = "Exit3|Round")
	int32 GetAnomalyActorCount() const { return AnomalyActors.Num(); }

	UPROPERTY(BlueprintAssignable, Category = "Exit3|Round")
	FExit3AnomalyActivated OnAnomalyActivated;

private:
	void RefreshAnomalyActors();
	void ActivateRandomAnomaly(EExit3Stage Stage);
	bool ValidateAnomalyActor(const AExit3AnomalyActor* Anomaly, TSet<FName>& SeenIds) const;

	UPROPERTY()
	TArray<TObjectPtr<AExit3AnomalyActor>> AnomalyActors;

	UPROPERTY()
	TObjectPtr<AExit3AnomalyActor> ActiveAnomaly;

	TMap<EExit3Stage, FName> LastAnomalyIdByStage;
};
