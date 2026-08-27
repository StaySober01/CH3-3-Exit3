#include "Anomaly/Exit3AnomalyActor.h"

AExit3AnomalyActor::AExit3AnomalyActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AExit3AnomalyActor::ApplyAnomaly_Implementation()
{
	bApplied = true;
}

void AExit3AnomalyActor::ResetAnomaly_Implementation()
{
	bApplied = false;
}
