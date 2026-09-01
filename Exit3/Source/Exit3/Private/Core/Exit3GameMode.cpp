#include "Core/Exit3GameMode.h"

#include "Core/Exit3GameState.h"
#include "Gameplay/Exit3LevelStreamManager.h"
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
	if (!StageManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("Exit3StageManager is not present in %s. Anomaly preparation will be skipped."),
			*GetWorld()->GetMapName());
	}

	LevelStreamManager = Cast<AExit3LevelStreamManager>(
		UGameplayStatics::GetActorOfClass(this, AExit3LevelStreamManager::StaticClass()));
	if (LevelStreamManager)
	{
		LevelStreamManager->OnGameplayLevelReady.AddUniqueDynamic(this, &AExit3GameMode::FinishRoundPreparation);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Exit3LevelStreamManager is not present in %s. Rounds will start without level recreation."),
			*GetWorld()->GetMapName());
	}
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
	AExit3GameState* State = GetExit3GameState();
	if (!State)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot start an Exit 3 round without Exit3GameState."));
		return;
	}

	if (State->IsGameCleared())
	{
		UE_LOG(LogTemp, Warning, TEXT("StartRound was ignored because the game is already cleared."));
		return;
	}

	bDecisionLocked = true;
	State->SetRoundState(FMath::RandBool() ? EExit3RoundState::Anomaly : EExit3RoundState::Normal);

	if (LevelStreamManager && LevelStreamManager->HasGameplayLevelConfigured())
	{
		UE_LOG(LogTemp, Log, TEXT("Round preparation started: Stage=%d, State=%s"),
			static_cast<uint8>(State->GetCurrentStage()),
			State->GetRoundState() == EExit3RoundState::Anomaly ? TEXT("Anomaly") : TEXT("Normal"));
		LevelStreamManager->RecreateGameplayLevel();
		return;
	}

	if (LevelStreamManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("Level streaming is bypassed because GameplayLevel is not configured."));
	}
	FinishRoundPreparation();
}

void AExit3GameMode::FinishRoundPreparation()
{
	AExit3GameState* State = GetExit3GameState();
	if (!State || State->IsGameCleared())
	{
		return;
	}

	if (StageManager)
	{
		StageManager->PrepareRound(State->GetCurrentStage(), State->GetRoundState());
	}

	bDecisionLocked = false;
	UE_LOG(LogTemp, Log, TEXT("Round started: Stage=%d, State=%s"),
		static_cast<uint8>(State->GetCurrentStage()),
		State->GetRoundState() == EExit3RoundState::Anomaly ? TEXT("Anomaly") : TEXT("Normal"));
	OnRoundStarted.Broadcast(State->GetCurrentStage(), State->GetRoundState());
}

void AExit3GameMode::SubmitDecision(const EExit3PlayerDecision Decision)
{
	if (bDecisionLocked)
	{
		UE_LOG(LogTemp, Verbose, TEXT("Decision ignored because the current round is locked."));
		return;
	}

	AExit3GameState* State = GetExit3GameState();
	if (!State)
	{
		UE_LOG(LogTemp, Error, TEXT("Decision ignored because Exit3GameState is unavailable."));
		return;
	}
	if (State->IsGameCleared())
	{
		UE_LOG(LogTemp, Verbose, TEXT("Decision ignored because the game is already cleared."));
		return;
	}
	bDecisionLocked = true;

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

	UE_LOG(LogTemp, Log, TEXT("Decision resolved: Decision=%s, Correct=%s, CurrentStage=%d, GameCleared=%s"),
		Decision == EExit3PlayerDecision::Anomaly ? TEXT("Anomaly") : TEXT("Normal"),
		bCorrect ? TEXT("true") : TEXT("false"),
		static_cast<uint8>(State->GetCurrentStage()),
		State->IsGameCleared() ? TEXT("true") : TEXT("false"));
	OnDecisionResolved.Broadcast(Decision, bCorrect, State->GetCurrentStage());

	if (!State->IsGameCleared())
	{
		StartRound();
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
