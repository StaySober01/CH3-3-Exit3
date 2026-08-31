#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Core/Exit3Types.h"
#include "Exit3GameMode.generated.h"

class AExit3StageManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FExit3DecisionResolved,
	EExit3PlayerDecision, Decision,
	bool, bCorrect,
	EExit3Stage, CurrentStage);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FExit3RoundStarted,
	EExit3Stage, Stage,
	EExit3RoundState, RoundState);

UCLASS()
class EXIT3_API AExit3GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AExit3GameMode();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Exit3|Game")
	void StartNewRun();

	UFUNCTION(BlueprintCallable, Category = "Exit3|Game")
	void StartRound();

	UFUNCTION(BlueprintCallable, Category = "Exit3|Game")
	void SubmitDecision(EExit3PlayerDecision Decision);

	UFUNCTION(BlueprintPure, Category = "Exit3|Game")
	bool IsDecisionLocked() const { return bDecisionLocked; }

	UPROPERTY(BlueprintAssignable, Category = "Exit3|Game")
	FExit3DecisionResolved OnDecisionResolved;

	UPROPERTY(BlueprintAssignable, Category = "Exit3|Game")
	FExit3RoundStarted OnRoundStarted;

private:
	void AdvanceStage();
	void ResetToStageOne();
	class AExit3GameState* GetExit3GameState() const;

	UPROPERTY()
	TObjectPtr<AExit3StageManager> StageManager;

	bool bDecisionLocked = false;
};
