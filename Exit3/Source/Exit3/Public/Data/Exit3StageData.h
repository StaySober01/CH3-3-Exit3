#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Core/Exit3Types.h"
#include "Exit3StageData.generated.h"

class AExit3AnomalyActor;

UCLASS(BlueprintType)
class EXIT3_API UExit3StageData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Exit3|Stage")
	EExit3Stage Stage = EExit3Stage::Stage1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Exit3|Stage")
	TArray<TSubclassOf<AExit3AnomalyActor>> AnomalyClasses;
};
