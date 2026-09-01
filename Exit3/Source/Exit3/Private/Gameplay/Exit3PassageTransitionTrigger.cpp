#include "Gameplay/Exit3PassageTransitionTrigger.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Core/Exit3GameMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Gameplay/Exit3LevelStreamManager.h"
#include "Kismet/GameplayStatics.h"
#include "Player/Exit3Character.h"

AExit3PassageTransitionTrigger::AExit3PassageTransitionTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	SetRootComponent(TriggerVolume);
	TriggerVolume->SetBoxExtent(FVector(100.0f, 250.0f, 150.0f));
	TriggerVolume->SetCollisionProfileName(TEXT("Trigger"));
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(
		this, &AExit3PassageTransitionTrigger::HandleBeginOverlap);

	TransitionSource = CreateDefaultSubobject<UArrowComponent>(TEXT("TransitionSource"));
	TransitionSource->SetupAttachment(TriggerVolume);
	TransitionSource->ArrowColor = FColor::Green;
	TransitionSource->ArrowSize = 2.0f;
}

void AExit3PassageTransitionTrigger::HandleBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	AExit3Character* Character = Cast<AExit3Character>(OtherActor);
	if (bConsumed || !Character)
	{
		return;
	}

	AExit3GameMode* GameMode = GetWorld()->GetAuthGameMode<AExit3GameMode>();
	if (!GameMode || GameMode->IsDecisionLocked())
	{
		return;
	}

	AExit3LevelStreamManager* StreamManager = Cast<AExit3LevelStreamManager>(
		UGameplayStatics::GetActorOfClass(this, AExit3LevelStreamManager::StaticClass()));
	if (!StreamManager)
	{
		UE_LOG(LogTemp, Error, TEXT("Passage transition failed: Exit3LevelStreamManager is unavailable."));
		return;
	}

	const EExit3PassageSide EntrySide = GameMode->GetCurrentRoundEntrySide();
	const EExit3PassageSide DestinationSide = EntrySide == EExit3PassageSide::SideA
		? EExit3PassageSide::SideB
		: EExit3PassageSide::SideA;
	const FTransform SourceTransform = TransitionSource->GetComponentTransform();
	const FTransform DestinationTransform = StreamManager->GetTransitionDestination(DestinationSide);

	const FVector LocalLocation = SourceTransform.InverseTransformPosition(Character->GetActorLocation());
	const FQuat LocalRotation = SourceTransform.GetRotation().Inverse() * Character->GetActorQuat();
	const FVector LocalVelocity = SourceTransform.InverseTransformVectorNoScale(Character->GetVelocity());

	const FVector DestinationLocation = DestinationTransform.TransformPosition(LocalLocation);
	const FQuat DestinationRotation = DestinationTransform.GetRotation() * LocalRotation;
	const FVector DestinationVelocity = DestinationTransform.TransformVectorNoScale(LocalVelocity);

	FRotator DestinationControlRotation = DestinationRotation.Rotator();
	if (AController* Controller = Character->GetController())
	{
		const FQuat LocalControlRotation =
			SourceTransform.GetRotation().Inverse() * Controller->GetControlRotation().Quaternion();
		DestinationControlRotation = (DestinationTransform.GetRotation() * LocalControlRotation).Rotator();
	}

	if (!Character->TeleportTo(DestinationLocation, DestinationRotation.Rotator(), false, false))
	{
		UE_LOG(LogTemp, Error, TEXT("Passage transition failed: destination %s is blocked."),
			DestinationSide == EExit3PassageSide::SideA ? TEXT("A") : TEXT("B"));
		return;
	}

	bConsumed = true;
	if (AController* Controller = Character->GetController())
	{
		Controller->SetControlRotation(DestinationControlRotation);
	}
	if (UCharacterMovementComponent* Movement = Character->GetCharacterMovement())
	{
		Movement->Velocity = DestinationVelocity;
	}

	UE_LOG(LogTemp, Log, TEXT("Passage transition completed: EntrySide=%s, DestinationSide=%s"),
		EntrySide == EExit3PassageSide::SideA ? TEXT("A") : TEXT("B"),
		DestinationSide == EExit3PassageSide::SideA ? TEXT("A") : TEXT("B"));

	GameMode->SubmitExitSide(DestinationSide);
}
