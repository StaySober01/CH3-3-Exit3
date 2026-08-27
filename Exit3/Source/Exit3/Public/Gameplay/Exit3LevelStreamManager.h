#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Exit3LevelStreamManager.generated.h"

class ULevelStreamingDynamic;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FExit3GameplayLevelReady);

UCLASS()
class EXIT3_API AExit3LevelStreamManager : public AActor
{
	GENERATED_BODY()

public:
	AExit3LevelStreamManager();

	UFUNCTION(BlueprintCallable, Category = "Exit3|Streaming")
	void RecreateGameplayLevel();

	UFUNCTION(BlueprintPure, Category = "Exit3|Streaming")
	bool IsGameplayLevelReady() const { return bGameplayLevelReady; }

	UPROPERTY(BlueprintAssignable, Category = "Exit3|Streaming")
	FExit3GameplayLevelReady OnGameplayLevelReady;

protected:
	UFUNCTION()
	void HandleGameplayLevelLoaded();

	UPROPERTY(EditAnywhere, Category = "Exit3|Streaming")
	TSoftObjectPtr<UWorld> GameplayLevel;

	UPROPERTY(EditAnywhere, Category = "Exit3|Streaming")
	FTransform GameplayLevelTransform;

private:
	UPROPERTY()
	TObjectPtr<ULevelStreamingDynamic> StreamingLevelInstance;

	bool bGameplayLevelReady = false;
};
