#include "Anomaly/Exit3AnomalyActor.h"

AExit3AnomalyActor::AExit3AnomalyActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

bool AExit3AnomalyActor::ActivateAnomaly()
{
	if (!bEnabled || bApplied)
	{
		return false;
	}

	if (!bNormalStateCaptured)
	{
		CaptureNormalState();
		bNormalStateCaptured = true;
	}

	ApplyAnomalyEffect();
	bApplied = true;
	return true;
}

void AExit3AnomalyActor::RestoreAnomaly()
{
	if (!bApplied)
	{
		return;
	}

	RestoreNormalState();
	bApplied = false;
}

void AExit3AnomalyActor::CaptureNormalState_Implementation()
{
}

void AExit3AnomalyActor::ApplyAnomalyEffect_Implementation()
{
}

void AExit3AnomalyActor::RestoreNormalState_Implementation()
{
}
