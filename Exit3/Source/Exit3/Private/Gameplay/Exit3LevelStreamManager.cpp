#include "Gameplay/Exit3LevelStreamManager.h"

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
}

void AExit3LevelStreamManager::RecreateGameplayLevel()
{
	bGameplayLevelReady = false;
	EntranceBlocker->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
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

	bool bLoadSucceeded = false;
	StreamingLevelInstance = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(
		this,
		GameplayLevel,
		GameplayLevelTransform.GetLocation(),
		GameplayLevelTransform.Rotator(),
		bLoadSucceeded);

	if (bLoadSucceeded && StreamingLevelInstance)
	{
		StreamingLevelInstance->OnLevelLoaded.AddDynamic(this, &AExit3LevelStreamManager::HandleGameplayLevelLoaded);
		UE_LOG(LogTemp, Log, TEXT("Gameplay level load requested: %s"), *GameplayLevel.ToSoftObjectPath().ToString());
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
	UE_LOG(LogTemp, Log, TEXT("Gameplay level is ready: %s"), *GameplayLevel.ToSoftObjectPath().ToString());
	OnGameplayLevelReady.Broadcast();
}
