#pragma once

#include "CoreMinimal.h"
#include "Exit3Types.generated.h"

UENUM(BlueprintType)
enum class EExit3Stage : uint8
{
	None = 0 UMETA(Hidden),
	Stage1 = 1 UMETA(DisplayName = "Stage 1"),
	Stage2 = 2 UMETA(DisplayName = "Stage 2"),
	Stage3 = 3 UMETA(DisplayName = "Stage 3")
};

UENUM(BlueprintType)
enum class EExit3RoundState : uint8
{
	Normal,
	Anomaly
};

UENUM(BlueprintType)
enum class EExit3PlayerDecision : uint8
{
	Normal,
	Anomaly
};

UENUM(BlueprintType)
enum class EExit3PassageSide : uint8
{
	SideA UMETA(DisplayName = "Side A"),
	SideB UMETA(DisplayName = "Side B")
};
