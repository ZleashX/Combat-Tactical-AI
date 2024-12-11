// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CEnum.h"


class TACTICSAI_API SpellUtils
{
	TArray<FIntPoint> GetPatternLine(FIntPoint Origin, int32 X, int32 Y) const;
	TArray<FIntPoint> GetPatternDiagonal(FIntPoint Origin, int32 X, int32 Y) const;
	TArray<FIntPoint> GetPatternStar(FIntPoint Origin, int32 X, int32 Y) const;
	TArray<FIntPoint> GetPatternDiamond(FIntPoint Origin, int32 X, int32 Y) const;
	TArray<FIntPoint> GetPatternSquare(FIntPoint Origin, int32 X, int32 Y) const;
	TArray<FIntPoint> GetOffsetIndexes(TArray<FIntPoint> Indexes, FIntPoint Offset);
public:
	TArray<FIntPoint> GetSpellIndexes(FIntPoint Origin, FIntPoint Range, ESpellRangePatternC Pattern);
};
