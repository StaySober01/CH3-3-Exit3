#include "Gameplay/Exit3LevelStreamManager.h"

#include "Engine/LevelStreamingDynamic.h"

AExit3LevelStreamManager::AExit3LevelStreamManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AExit3LevelStreamManager::RecreateGameplayLevel()
{
	bGameplayLevelReady = false;
	if (StreamingLevelInstance)
	{
		StreamingLevelInstance->SetShouldBeVisible(false);
		StreamingLevelInstance->SetShouldBeLoaded(false);
		StreamingLevelInstance->SetIsRequestingUnloadAndRemoval(true);
		StreamingLevelInstance = nullptr;
	}

	if (GameplayLevel.IsNull())
	{
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
	}
}

void AExit3LevelStreamManager::HandleGameplayLevelLoaded()
{
	bGameplayLevelReady = true;
	OnGameplayLevelReady.Broadcast();
}
