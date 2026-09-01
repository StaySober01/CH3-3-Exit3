#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Exit3PassageTransitionTrigger.generated.h"

class UArrowComponent;
class UBoxComponent;

UCLASS()
class EXIT3_API AExit3PassageTransitionTrigger : public AActor
{
	GENERATED_BODY()

public:
	AExit3PassageTransitionTrigger();

protected:
	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Exit3|Transition")
	TObjectPtr<UBoxComponent> TriggerVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Exit3|Transition")
	TObjectPtr<UArrowComponent> TransitionSource;

private:
	bool bConsumed = false;
};
