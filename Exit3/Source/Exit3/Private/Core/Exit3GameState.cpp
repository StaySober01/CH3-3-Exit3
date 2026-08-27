#include "Core/Exit3GameState.h"

void AExit3GameState::SetCurrentStage(const EExit3Stage NewStage)
{
	CurrentStage = NewStage;
	OnStageChanged.Broadcast(CurrentStage);
}

void AExit3GameState::SetRoundState(const EExit3RoundState NewRoundState)
{
	RoundState = NewRoundState;
}

void AExit3GameState::SetGameCleared(const bool bCleared)
{
	bGameCleared = bCleared;
	if (bGameCleared)
	{
		OnGameCleared.Broadcast();
	}
}
