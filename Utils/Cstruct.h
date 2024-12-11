// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CEnum.h"
#include "Cstruct.generated.h"

USTRUCT(BlueprintType)
struct FSpellData
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	bool IsDamaging;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	int32 Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	FIntPoint Range;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	int32 ApCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	int32 Cooldown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	int32 CooldownLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	ESpellRangePatternC Pattern;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	bool bIsAOE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	FIntPoint AoeRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	ESpellRangePatternC AoePattern;

	bool operator==(const FSpellData& Other) const
	{
		return Name == Other.Name &&
			IsDamaging == Other.IsDamaging &&
			Damage == Other.Damage &&
			Range == Other.Range &&
			ApCost == Other.ApCost &&
			Cooldown == Other.Cooldown &&
			Pattern == Other.Pattern &&
			bIsAOE == Other.bIsAOE &&
			AoeRange == Other.AoeRange &&
			AoePattern == Other.AoePattern;
	}

};

USTRUCT(BlueprintType)
struct FEntityData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntPoint Position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Ap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxAp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MovementRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TeamIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSpellData> Spells;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ETileTypeC> ValidTileType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsCrystal;

	bool IsUnitAlive() const { return Health > 0; }

	bool operator==(const FEntityData& Other) const
	{
		return Position == Other.Position &&
			Health == Other.Health &&
			MaxHealth == Other.MaxHealth &&
			Ap == Other.Ap &&
			MovementRange == Other.MovementRange &&
			TeamIndex == Other.TeamIndex;
	}

	void ApplyDamage(int32 Damage)
	{
		Health += Damage;
		Health = FMath::Clamp(Health, 0, MaxHealth);
	}

	void ResolveTurn()
	{
		Ap = MaxAp;
		for (auto& Spell : Spells)
		{
			if (Spell.CooldownLeft > 0)
			{
				Spell.CooldownLeft--;
			}
		}
	}
};

USTRUCT(BlueprintType)
struct FMctsAction
{
	GENERATED_BODY()

	FMctsAction() : ActionType(EActionType::None), Target(FIntPoint()), Spell(FSpellData()) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EActionType ActionType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntPoint Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSpellData Spell;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FIntPoint> AoeTarget;

	void Init(EActionType AT, FIntPoint T, FSpellData S = FSpellData(), TArray<FIntPoint> AOE = TArray<FIntPoint>())
	{ 
		ActionType = AT; 
		Target = T; 
		Spell = S; 
		AoeTarget = AOE;
	}

	void operator=(const FMctsAction& other) 
	{ 
		ActionType = other.ActionType;
		Target = other.Target; 
		Spell = other.Spell; 
		AoeTarget = other.AoeTarget;
	}

	bool operator==(const FMctsAction& other) const 
	{ 
		return ActionType == other.ActionType &&
			Target == other.Target && 
			Spell == other.Spell &&
			AoeTarget == other.AoeTarget; 
	}
};

USTRUCT()
struct FEvalData
{
	GENERATED_BODY()
	FEvalData() : OriEnemyHealth(0), OriAllyHealth(0), OriCrystalHealth(0), OriCrystalDist(0), OriEnemyDist(0), OriEnemyCount(0), OriAllyCount(0) {}
	int32 OriEnemyHealth;
	int32 OriAllyHealth;
	int32 OriCrystalHealth;
	int32 OriCrystalDist;
	int32 OriEnemyDist;
	int32 OriEnemyCount;
	int32 OriAllyCount;
	int32 OriCrystalCount;
};