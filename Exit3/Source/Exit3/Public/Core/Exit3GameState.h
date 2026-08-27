#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Core/Exit3Types.h"
#include "Exit3GameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FExit3StageChanged, EExit3Stage, NewStage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FExit3GameCleared);

UCLASS()
class EXIT3_API AExit3GameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Exit3|Stage")
	EExit3Stage GetCurrentStage() const { return CurrentStage; }

	UFUNCTION(BlueprintPure, Category = "Exit3|Round")
	EExit3RoundState GetRoundState() const { return RoundState; }

	UFUNCTION(BlueprintPure, Category = "Exit3|Game")
	bool IsGameCleared() const { return bGameCleared; }

	void SetCurrentStage(EExit3Stage NewStage);
	void SetRoundState(EExit3RoundState NewRoundState);
	void SetGameCleared(bool bCleared);

	UPROPERTY(BlueprintAssignable, Category = "Exit3|Stage")
	FExit3StageChanged OnStageChanged;

	UPROPERTY(BlueprintAssignable, Category = "Exit3|Game")
	FExit3GameCleared OnGameCleared;

private:
	UPROPERTY(VisibleAnywhere, Category = "Exit3|Stage")
	EExit3Stage CurrentStage = EExit3Stage::Stage1;

	UPROPERTY(VisibleAnywhere, Category = "Exit3|Round")
	EExit3RoundState RoundState = EExit3RoundState::Normal;

	UPROPERTY(VisibleAnywhere, Category = "Exit3|Game")
	bool bGameCleared = false;
};
