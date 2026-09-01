#include "Gameplay/Exit3StageManager.h"

#include "Anomaly/Exit3AnomalyActor.h"
#include "EngineUtils.h"

AExit3StageManager::AExit3StageManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AExit3StageManager::PrepareRound(const EExit3Stage Stage, const EExit3RoundState RoundState)
{
	RefreshAnomalyActors();
	ResetRound();

	UE_LOG(LogTemp, Log, TEXT("Anomaly candidates refreshed: Total=%d, Stage=%d, RoundState=%s"),
		AnomalyActors.Num(),
		static_cast<uint8>(Stage),
		RoundState == EExit3RoundState::Anomaly ? TEXT("Anomaly") : TEXT("Normal"));

	if (RoundState == EExit3RoundState::Anomaly)
	{
		ActivateRandomAnomaly(Stage);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Normal round prepared with no active anomaly."));
	}
}

void AExit3StageManager::ResetRound()
{
	for (AExit3AnomalyActor* Anomaly : AnomalyActors)
	{
		if (IsValid(Anomaly))
		{
			Anomaly->RestoreAnomaly();
		}
	}
	ActiveAnomaly = nullptr;
}

void AExit3StageManager::RefreshAnomalyActors()
{
	AnomalyActors.Reset();
	TSet<FName> SeenIds;
	for (TActorIterator<AExit3AnomalyActor> It(GetWorld()); It; ++It)
	{
		AExit3AnomalyActor* Anomaly = *It;
		if (ValidateAnomalyActor(Anomaly, SeenIds))
		{
			AnomalyActors.Add(Anomaly);
		}
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
		UE_LOG(LogTemp, Warning, TEXT("No valid anomaly candidates were found for Stage %d."), static_cast<uint8>(Stage));
		return;
	}

	const FName PreviousId = LastAnomalyIdByStage.FindRef(Stage);
	if (Candidates.Num() > 1 && !PreviousId.IsNone())
	{
		Candidates.RemoveAll([PreviousId](const AExit3AnomalyActor* Candidate)
		{
			return Candidate && Candidate->GetAnomalyId() == PreviousId;
		});
	}

	ActiveAnomaly = Candidates[FMath::RandRange(0, Candidates.Num() - 1)];
	if (!ActiveAnomaly->ActivateAnomaly())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to activate anomaly '%s'."), *ActiveAnomaly->GetAnomalyId().ToString());
		ActiveAnomaly = nullptr;
		return;
	}

	LastAnomalyIdByStage.Add(Stage, ActiveAnomaly->GetAnomalyId());
	UE_LOG(LogTemp, Log, TEXT("Anomaly activated: Id=%s, Stage=%d, CandidateCount=%d"),
		*ActiveAnomaly->GetAnomalyId().ToString(),
		static_cast<uint8>(Stage),
		Candidates.Num());
	OnAnomalyActivated.Broadcast(ActiveAnomaly->GetAnomalyId(), Stage, ActiveAnomaly);
}

FName AExit3StageManager::GetActiveAnomalyId() const
{
	return IsValid(ActiveAnomaly) ? ActiveAnomaly->GetAnomalyId() : NAME_None;
}

bool AExit3StageManager::ValidateAnomalyActor(const AExit3AnomalyActor* Anomaly, TSet<FName>& SeenIds) const
{
	if (!IsValid(Anomaly) || !Anomaly->IsEnabled())
	{
		return false;
	}

	const FName Id = Anomaly->GetAnomalyId();
	if (Id.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("Anomaly actor '%s' was ignored because AnomalyId is None."), *Anomaly->GetName());
		return false;
	}

	if (Anomaly->GetSupportedStage() == EExit3Stage::None)
	{
		UE_LOG(LogTemp, Warning, TEXT("Anomaly '%s' was ignored because SupportedStage is None."), *Id.ToString());
		return false;
	}

	if (SeenIds.Contains(Id))
	{
		UE_LOG(LogTemp, Warning, TEXT("Duplicate anomaly id '%s' was ignored. Anomaly IDs must be unique."), *Id.ToString());
		return false;
	}

	SeenIds.Add(Id);
	return true;
}
