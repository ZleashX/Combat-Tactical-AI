// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Containers/Queue.h"
#include "../Utils/Cstruct.h"
#include "../Utils/CEnum.h"
#include "../Utils/SpellUtils.h"

class UMctsState 
{
	TSharedPtr<TMap<FIntPoint, ETileTypeC>> Grid;

	int32 CurUnitIndex;

	int32 OriUnitIndex;

	int32 OriCrystalIndex;

	int32 Turn;

	EUtilityGoals Goal;

	FEvalData EvalData;

	int32 WinningTeam;

	bool bAiTurn;

	bool IsPositionOccupied(FIntPoint Position, bool bCheckForEnemy = false, bool bCheckForTeam = false) const;
	int32 GetDistanceToTarget(FIntPoint StartIndex, FIntPoint TargetIndex, TArray<ETileTypeC> ValidTileType);
	TArray<FIntPoint> GetValidMovesIndexes(FIntPoint StartIndex, int32 MoveRange, TArray<ETileTypeC> ValidTileType);

public:

	TArray<FEntityData> Units;
	UMctsState();
	~UMctsState();
	void Init(TArray<FEntityData> U, TSharedPtr<TMap<FIntPoint, ETileTypeC>> G, FEvalData ED, int32 CUI, int32 OI, int32 OCI, int32 T, EUtilityGoals UG, bool bAT);

	void InitStartState(TArray<FEntityData> U, int32 CUI, EUtilityGoals UG, TSharedPtr<TMap<FIntPoint, ETileTypeC>> GridPtr);
	void ActionsToTry(TQueue<FMctsAction>& Moves);
	TArray<FMctsAction> ArrayActionsToTry();
	UMctsState* NextState(const FMctsAction Move) const;
	double Rollout();
	double Evaluate();
	void CalculateWinners();
	bool IsTerminal() const;
	void Print() const;
	bool IsAiTurn() const;
};
