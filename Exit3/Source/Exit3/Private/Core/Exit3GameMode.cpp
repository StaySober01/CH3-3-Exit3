#include "Core/Exit3GameMode.h"

#include "Core/Exit3GameState.h"
#include "Gameplay/Exit3StageManager.h"
#include "Kismet/GameplayStatics.h"
#include "Player/Exit3Character.h"
#include "Player/Exit3PlayerController.h"

AExit3GameMode::AExit3GameMode()
{
	GameStateClass = AExit3GameState::StaticClass();
	DefaultPawnClass = AExit3Character::StaticClass();
	PlayerControllerClass = AExit3PlayerController::StaticClass();
}

void AExit3GameMode::BeginPlay()
{
	Super::BeginPlay();
	StageManager = Cast<AExit3StageManager>(UGameplayStatics::GetActorOfClass(this, AExit3StageManager::StaticClass()));
	StartNewRun();
}

void AExit3GameMode::StartNewRun()
{
	if (AExit3GameState* State = GetExit3GameState())
	{
		State->SetGameCleared(false);
		State->SetCurrentStage(EExit3Stage::Stage1);
	}
	StartRound();
}

void AExit3GameMode::StartRound()
{
	bDecisionLocked = false;
	AExit3GameState* State = GetExit3GameState();
	if (!State)
	{
		return;
	}

	State->SetRoundState(FMath::RandBool() ? EExit3RoundState::Anomaly : EExit3RoundState::Normal);
	if (StageManager)
	{
		StageManager->PrepareRound(State->GetCurrentStage(), State->GetRoundState());
	}
}

void AExit3GameMode::SubmitDecision(const EExit3PlayerDecision Decision)
{
	if (bDecisionLocked)
	{
		return;
	}
	bDecisionLocked = true;

	AExit3GameState* State = GetExit3GameState();
	if (!State)
	{
		return;
	}

	const bool bCorrect =
		(Decision == EExit3PlayerDecision::Normal && State->GetRoundState() == EExit3RoundState::Normal) ||
		(Decision == EExit3PlayerDecision::Anomaly && State->GetRoundState() == EExit3RoundState::Anomaly);

	if (bCorrect)
	{
		AdvanceStage();
	}
	else
	{
		ResetToStageOne();
	}
}

void AExit3GameMode::AdvanceStage()
{
	AExit3GameState* State = GetExit3GameState();
	if (!State)
	{
		return;
	}

	switch (State->GetCurrentStage())
	{
	case EExit3Stage::Stage1:
		State->SetCurrentStage(EExit3Stage::Stage2);
		break;
	case EExit3Stage::Stage2:
		State->SetCurrentStage(EExit3Stage::Stage3);
		break;
	case EExit3Stage::Stage3:
		State->SetGameCleared(true);
		break;
	default:
		break;
	}
}

void AExit3GameMode::ResetToStageOne()
{
	if (AExit3GameState* State = GetExit3GameState())
	{
		State->SetCurrentStage(EExit3Stage::Stage1);
	}
}

AExit3GameState* AExit3GameMode::GetExit3GameState() const
{
	return GetGameState<AExit3GameState>();
}
