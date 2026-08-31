#include "Gameplay/Exit3ChoiceTrigger.h"

#include "Components/BoxComponent.h"
#include "Core/Exit3GameMode.h"
#include "Player/Exit3Character.h"

AExit3ChoiceTrigger::AExit3ChoiceTrigger()
{
	PrimaryActorTick.bCanEverTick = false;
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	SetRootComponent(TriggerVolume);
	TriggerVolume->SetCollisionProfileName(TEXT("Trigger"));
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AExit3ChoiceTrigger::HandleBeginOverlap);
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &AExit3ChoiceTrigger::HandleEndOverlap);
}

void AExit3ChoiceTrigger::ResetTrigger()
{
	bConsumed = false;
}

void AExit3ChoiceTrigger::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (bConsumed || !OtherActor || !OtherActor->IsA<AExit3Character>())
	{
		return;
	}

	if (AExit3GameMode* GameMode = GetWorld()->GetAuthGameMode<AExit3GameMode>())
	{
		bConsumed = true;
		GameMode->SubmitDecision(Decision);
	}
}

void AExit3ChoiceTrigger::HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->IsA<AExit3Character>())
	{
		ResetTrigger();
	}
}
