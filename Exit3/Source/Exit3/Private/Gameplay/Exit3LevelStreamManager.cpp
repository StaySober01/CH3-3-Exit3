#include "Gameplay/Exit3LevelStreamManager.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/LevelStreamingDynamic.h"

AExit3LevelStreamManager::AExit3LevelStreamManager()
{
	PrimaryActorTick.bCanEverTick = false;

	EntranceBlocker = CreateDefaultSubobject<UBoxComponent>(TEXT("EntranceBlocker"));
	SetRootComponent(EntranceBlocker);
	EntranceBlocker->SetBoxExtent(FVector(30.0f, 250.0f, 150.0f));
	EntranceBlocker->SetCollisionObjectType(ECC_WorldStatic);
	EntranceBlocker->SetCollisionResponseToAllChannels(ECR_Ignore);
	EntranceBlocker->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	EntranceBlocker->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	EntranceBlockerB = CreateDefaultSubobject<UBoxComponent>(TEXT("EntranceBlockerB"));
	EntranceBlockerB->SetupAttachment(EntranceBlocker);
	EntranceBlockerB->SetBoxExtent(FVector(30.0f, 250.0f, 150.0f));
	EntranceBlockerB->SetCollisionObjectType(ECC_WorldStatic);
	EntranceBlockerB->SetCollisionResponseToAllChannels(ECR_Ignore);
	EntranceBlockerB->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	EntranceBlockerB->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TransitionDestinationA = CreateDefaultSubobject<UArrowComponent>(TEXT("TransitionDestinationA"));
	TransitionDestinationA->SetupAttachment(EntranceBlocker);
	TransitionDestinationA->ArrowColor = FColor::Cyan;
	TransitionDestinationA->ArrowSize = 2.0f;

	TransitionDestinationB = CreateDefaultSubobject<UArrowComponent>(TEXT("TransitionDestinationB"));
	TransitionDestinationB->SetupAttachment(EntranceBlocker);
	TransitionDestinationB->ArrowColor = FColor::Yellow;
	TransitionDestinationB->ArrowSize = 2.0f;
}

FTransform AExit3LevelStreamManager::GetTransitionDestination(const EExit3PassageSide Side) const
{
	return Side == EExit3PassageSide::SideA
		? TransitionDestinationA->GetComponentTransform()
		: TransitionDestinationB->GetComponentTransform();
}

void AExit3LevelStreamManager::RecreateGameplayLevel()
{
	RecreateGameplayLevelForEntry(EExit3PassageSide::SideA);
}

void AExit3LevelStreamManager::RecreateGameplayLevelForEntry(const EExit3PassageSide EntrySide)
{
	ActiveEntrySide = EntrySide;
	bGameplayLevelReady = false;
	EntranceBlocker->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	EntranceBlockerB->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	OnGameplayLevelLoading.Broadcast();

	if (StreamingLevelInstance)
	{
		StreamingLevelInstance->OnLevelLoaded.RemoveDynamic(this, &AExit3LevelStreamManager::HandleGameplayLevelLoaded);
		StreamingLevelInstance->SetShouldBeVisible(false);
		StreamingLevelInstance->SetShouldBeLoaded(false);
		StreamingLevelInstance->SetIsRequestingUnloadAndRemoval(true);
		StreamingLevelInstance = nullptr;
	}

	if (GameplayLevel.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("Gameplay level recreation failed: no GameplayLevel is configured on %s."), *GetName());
		return;
	}

	const FTransform& SpawnTransform =
		ActiveEntrySide == EExit3PassageSide::SideB
		? GameplayLevelTransformSideB
		: GameplayLevelTransform;

	bool bLoadSucceeded = false;
	StreamingLevelInstance = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(
		this,
		GameplayLevel,
		SpawnTransform.GetLocation(),
		SpawnTransform.Rotator(),
		bLoadSucceeded);

	if (bLoadSucceeded && StreamingLevelInstance)
	{
		StreamingLevelInstance->OnLevelLoaded.AddDynamic(this, &AExit3LevelStreamManager::HandleGameplayLevelLoaded);
		UE_LOG(LogTemp, Log, TEXT("Gameplay level load requested: %s, EntrySide=%s, Location=%s, Rotation=%s"),
			*GameplayLevel.ToSoftObjectPath().ToString(),
			ActiveEntrySide == EExit3PassageSide::SideA ? TEXT("A") : TEXT("B"),
			*SpawnTransform.GetLocation().ToCompactString(),
			*SpawnTransform.Rotator().ToCompactString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Gameplay level load request failed: %s"), *GameplayLevel.ToSoftObjectPath().ToString());
	}
}

void AExit3LevelStreamManager::HandleGameplayLevelLoaded()
{
	if (StreamingLevelInstance)
	{
		StreamingLevelInstance->OnLevelLoaded.RemoveDynamic(this, &AExit3LevelStreamManager::HandleGameplayLevelLoaded);
	}
	bGameplayLevelReady = true;
	EntranceBlocker->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EntranceBlockerB->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UE_LOG(LogTemp, Log, TEXT("Gameplay level is ready: %s"), *GameplayLevel.ToSoftObjectPath().ToString());
	OnGameplayLevelReady.Broadcast();
}
