#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ESpellRangePatternC : uint8
{
	Line UMETA(DisplayName = "Line"),
	Diagonal UMETA(DisplayName = "Diagonal"),
	Star UMETA(DisplayName = "Star"),
	Diamond UMETA(DisplayName = "Diamond"),
	Square UMETA(DisplayName = "Square"),
};

UENUM(BlueprintType)
enum class ETileTypeC : uint8
{
	None UMETA(DisplayName = "None"),
	Normal UMETA(DisplayName = "Normal"),
	Obstacle UMETA(DisplayName = "Obstacle"),
	DoubleCost UMETA(DisplayName = "DoubleCost"),
	TripleCost UMETA(DisplayName = "TripleCost"),
	FlyingUnitsOnly UMETA(DisplayName = "FlyingUnitsOnly"),
};

UENUM(BlueprintType)
enum class EUtilityGoals : uint8
{
	None UMETA(DisplayName = "None"),
	AttackCrystal UMETA(DisplayName = "Attack Crystal"),
	Retreat UMETA(DisplayName = "Retreat"),
	KillingSpree UMETA(DisplayName = "Killing Spree"),
};

UENUM(BlueprintType)
enum class EActionType : uint8
{
	None UMETA(DisplayName = "None"),
	Move UMETA(DisplayName = "Move"),
	Spell UMETA(DisplayName = "Spell"),
	EndTurn UMETA(DisplayName = "EndTurn"),
};