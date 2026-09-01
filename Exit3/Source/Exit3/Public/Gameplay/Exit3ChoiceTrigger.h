#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Exit3Types.h"
#include "Exit3ChoiceTrigger.generated.h"

class UBoxComponent;
class UArrowComponent;

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

	UPROPERTY(VisibleAnywhere, Category = "Exit3|Trigger")
	TObjectPtr<UArrowComponent> ExitDirection;

	UPROPERTY(EditAnywhere, Category = "Exit3|Trigger")
	EExit3PassageSide ExitSide = EExit3PassageSide::SideA;

	UPROPERTY(EditAnywhere, Category = "Exit3|Trigger")
	bool bRequireExitDirection = true;

	UPROPERTY(EditAnywhere, Category = "Exit3|Trigger", meta = (ClampMin = "-1.0", ClampMax = "1.0", EditCondition = "bRequireExitDirection"))
	float MinimumExitDirectionDot = 0.25f;

private:
	bool bConsumed = false;
};
