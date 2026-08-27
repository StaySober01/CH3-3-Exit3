#include "Gameplay/Exit3StageManager.h"

#include "Anomaly/Exit3AnomalyActor.h"
#include "EngineUtils.h"

AExit3StageManager::AExit3StageManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AExit3StageManager::PrepareRound(const EExit3Stage Stage, const EExit3RoundState RoundState)
{
	ResetRound();
	RefreshAnomalyActors();
	if (RoundState == EExit3RoundState::Anomaly)
	{
		ActivateRandomAnomaly(Stage);
	}
}

void AExit3StageManager::ResetRound()
{
	for (AExit3AnomalyActor* Anomaly : AnomalyActors)
	{
		if (IsValid(Anomaly))
		{
			Anomaly->ResetAnomaly();
		}
	}
	ActiveAnomaly = nullptr;
}

void AExit3StageManager::RefreshAnomalyActors()
{
	AnomalyActors.Reset();
	for (TActorIterator<AExit3AnomalyActor> It(GetWorld()); It; ++It)
	{
		AnomalyActors.Add(*It);
	}
}

void AExit3StageManager::ActivateRandomAnomaly(const EExit3Stage Stage)
{
	TArray<AExit3AnomalyActor*> Candidates;
	for (AExit3AnomalyActor* Anomaly : AnomalyActors)
	{
		if (IsValid(Anomaly) && Anomaly->GetSupportedStage() == Stage)
		{
			Candidates.Add(Anomaly);
		}
	}

	if (Candidates.IsEmpty())
	{
		return;
	}

	ActiveAnomaly = Candidates[FMath::RandRange(0, Candidates.Num() - 1)];
	ActiveAnomaly->ApplyAnomaly();
}
