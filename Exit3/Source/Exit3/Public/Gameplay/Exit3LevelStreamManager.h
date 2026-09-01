#pragma once

#include "CoreMinimal.h"
#include "Core/Exit3Types.h"
#include "GameFramework/Actor.h"
#include "Exit3LevelStreamManager.generated.h"

class ULevelStreamingDynamic;
class UBoxComponent;
class UArrowComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FExit3GameplayLevelReady);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FExit3GameplayLevelLoading);

UCLASS()
class EXIT3_API AExit3LevelStreamManager : public AActor
{
	GENERATED_BODY()

public:
	AExit3LevelStreamManager();

	UFUNCTION(BlueprintCallable, Category = "Exit3|Streaming")
	void RecreateGameplayLevel();

	UFUNCTION(BlueprintCallable, Category = "Exit3|Streaming")
	void RecreateGameplayLevelForEntry(EExit3PassageSide EntrySide);

	UFUNCTION(BlueprintPure, Category = "Exit3|Streaming")
	bool IsGameplayLevelReady() const { return bGameplayLevelReady; }

	UFUNCTION(BlueprintPure, Category = "Exit3|Streaming")
	bool HasGameplayLevelConfigured() const { return !GameplayLevel.IsNull(); }

	UFUNCTION(BlueprintPure, Category = "Exit3|Streaming")
	FTransform GetTransitionDestination(EExit3PassageSide Side) const;

	UPROPERTY(BlueprintAssignable, Category = "Exit3|Streaming")
	FExit3GameplayLevelLoading OnGameplayLevelLoading;

	UPROPERTY(BlueprintAssignable, Category = "Exit3|Streaming")
	FExit3GameplayLevelReady OnGameplayLevelReady;

protected:
	UFUNCTION()
	void HandleGameplayLevelShown();

	UPROPERTY(EditAnywhere, Category = "Exit3|Streaming")
	TSoftObjectPtr<UWorld> GameplayLevel;

	UPROPERTY(EditAnywhere, Category = "Exit3|Streaming")
	FTransform GameplayLevelTransform;

	UPROPERTY(EditAnywhere, Category = "Exit3|Streaming")
	FTransform GameplayLevelTransformSideB;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Exit3|Streaming", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> EntranceBlocker;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Exit3|Streaming", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> EntranceBlockerB;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Exit3|Streaming", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UArrowComponent> TransitionDestinationA;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Exit3|Streaming", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UArrowComponent> TransitionDestinationB;

private:
	UPROPERTY()
	TObjectPtr<ULevelStreamingDynamic> StreamingLevelInstance;

	EExit3PassageSide ActiveEntrySide = EExit3PassageSide::SideA;
	bool bGameplayLevelReady = false;
};
