#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Exit3Types.h"
#include "Exit3ChoiceTrigger.generated.h"

class UBoxComponent;

UCLASS()
class EXIT3_API AExit3ChoiceTrigger : public AActor
{
	GENERATED_BODY()

public:
	AExit3ChoiceTrigger();

	UFUNCTION(BlueprintCallable, Category = "Exit3|Trigger")
	void ResetTrigger();

protected:
	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, Category = "Exit3|Trigger")
	TObjectPtr<UBoxComponent> TriggerVolume;

	UPROPERTY(EditAnywhere, Category = "Exit3|Trigger")
	EExit3PlayerDecision Decision = EExit3PlayerDecision::Normal;

private:
	bool bConsumed = false;
};
