#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Core/Exit3Types.h"
#include "Exit3GameMode.generated.h"

class AExit3StageManager;
class AExit3LevelStreamManager;

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

	UFUNCTION(BlueprintCallable, Category = "Exit3|Game")
	void SubmitExitSide(EExit3PassageSide ExitSide);

	UFUNCTION(BlueprintPure, Category = "Exit3|Game")
	bool IsDecisionLocked() const { return bDecisionLocked; }

	UFUNCTION(BlueprintPure, Category = "Exit3|Game")
	EExit3PassageSide GetCurrentRoundEntrySide() const { return CurrentRoundEntrySide; }

	UPROPERTY(BlueprintAssignable, Category = "Exit3|Game")
	FExit3DecisionResolved OnDecisionResolved;

	UPROPERTY(BlueprintAssignable, Category = "Exit3|Game")
	FExit3RoundStarted OnRoundStarted;

private:
	UFUNCTION()
	void FinishRoundPreparation();

	void AdvanceStage();
	void ResetToStageOne();
	static EExit3PassageSide GetOppositeSide(EExit3PassageSide Side);
	class AExit3GameState* GetExit3GameState() const;

	UPROPERTY()
	TObjectPtr<AExit3StageManager> StageManager;

	UPROPERTY()
	TObjectPtr<AExit3LevelStreamManager> LevelStreamManager;

	UPROPERTY(EditDefaultsOnly, Category = "Exit3|Passage")
	EExit3PassageSide InitialEntrySide = EExit3PassageSide::SideA;

	EExit3PassageSide CurrentRoundEntrySide = EExit3PassageSide::SideA;

	bool bDecisionLocked = false;
};
