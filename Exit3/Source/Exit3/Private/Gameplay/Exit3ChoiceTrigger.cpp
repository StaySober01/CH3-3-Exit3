#include "Gameplay/Exit3ChoiceTrigger.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Core/Exit3GameMode.h"
#include "Player/Exit3Character.h"

AExit3ChoiceTrigger::AExit3ChoiceTrigger()
{
	PrimaryActorTick.bCanEverTick = false;
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	SetRootComponent(TriggerVolume);
	ExitDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("ExitDirection"));
	ExitDirection->SetupAttachment(TriggerVolume);
	ExitDirection->ArrowColor = FColor::Green;
	ExitDirection->ArrowSize = 1.5f;
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
	AExit3Character* Character = Cast<AExit3Character>(OtherActor);
	if (bConsumed || !Character)
	{
		return;
	}

	if (bRequireExitDirection)
	{
		const FVector MovementDirection = Character->GetVelocity().GetSafeNormal2D();
		const FVector RequiredDirection = ExitDirection->GetForwardVector().GetSafeNormal2D();
		if (MovementDirection.IsNearlyZero() || FVector::DotProduct(MovementDirection, RequiredDirection) < MinimumExitDirectionDot)
		{
			return;
		}
	}

	if (AExit3GameMode* GameMode = GetWorld()->GetAuthGameMode<AExit3GameMode>())
	{
		bConsumed = true;
		GameMode->SubmitExitSide(ExitSide);
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
