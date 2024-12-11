// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellUtils.h"

TArray<FIntPoint> SpellUtils::GetSpellIndexes(FIntPoint Origin, FIntPoint Range, ESpellRangePatternC Pattern)
{
	TArray<FIntPoint> Indexes;
	switch (Pattern)
	{
	case ESpellRangePatternC::Line:
		Indexes = GetPatternLine(Origin, Range.X, Range.Y);
		break;
	case ESpellRangePatternC::Diagonal:
		Indexes = GetPatternDiagonal(Origin, Range.X, Range.Y);
		break;
	case ESpellRangePatternC::Star:
		Indexes = GetPatternStar(Origin, Range.X, Range.Y);
		break;
	case ESpellRangePatternC::Diamond:
		Indexes = GetPatternDiamond(Origin, Range.X, Range.Y);
		break;
	case ESpellRangePatternC::Square:
		Indexes = GetPatternSquare(Origin, Range.X, Range.Y);
		break;
	default:
		break;
	}
	Indexes = GetOffsetIndexes(Indexes, Origin);
	return Indexes;
}

TArray<FIntPoint> SpellUtils::GetPatternLine(FIntPoint Origin, int32 X, int32 Y) const
{
	TArray<FIntPoint> Result;
	for (int32 i = X; i <= Y; i++)
	{
		Result.AddUnique(FIntPoint(0, i));
		Result.AddUnique(FIntPoint(0, -i));
		Result.AddUnique(FIntPoint(i, 0));
		Result.AddUnique(FIntPoint(-i, 0));
	}
	return Result;
}

TArray<FIntPoint> SpellUtils::GetPatternDiagonal(FIntPoint Origin, int32 X, int32 Y) const
{
	TArray<FIntPoint> Result;
	for (int32 i = X; i <= Y; i++)
	{
		Result.AddUnique(FIntPoint(i, i));
		Result.AddUnique(FIntPoint(-i, -i));
		Result.AddUnique(FIntPoint(i, -i));
		Result.AddUnique(FIntPoint(-i, i));
	}
	return Result;
}

TArray<FIntPoint> SpellUtils::GetPatternStar(FIntPoint Origin, int32 X, int32 Y) const
{
	TArray<FIntPoint> Result;
	Result = GetPatternLine(Origin, X, Y);
	Result.Append(GetPatternDiagonal(Origin, X, Y));
	return Result;
}

TArray<FIntPoint> SpellUtils::GetPatternDiamond(FIntPoint Origin, int32 X, int32 Y) const
{
	TArray<FIntPoint> Result;
	for (int32 i = X; i <= Y; i++)
	{
		for (int32 j = 0; j < i; j++)
		{
			Result.AddUnique(FIntPoint(j,-(i - j)));
			Result.AddUnique(FIntPoint(i - j, j));
			Result.AddUnique(FIntPoint(-j, i - j));
			Result.AddUnique(FIntPoint(-(i - j), -j));
		}
	}
	return Result;
}

TArray<FIntPoint> SpellUtils::GetPatternSquare(FIntPoint Origin, int32 X, int32 Y) const
{
	TArray<FIntPoint> Result;
	for (int32 i = X; i <= Y; i++)
	{
		for (int32 j = -i; j <= i; j++)
		{
			Result.AddUnique(FIntPoint(j, -i));
			Result.AddUnique(FIntPoint(i, j));
			Result.AddUnique(FIntPoint(-j, i));
			Result.AddUnique(FIntPoint(-i, -j));
		}
	}
	return Result;
}

TArray<FIntPoint> SpellUtils::GetOffsetIndexes(TArray<FIntPoint> Indexes, FIntPoint Offset)
{
	TArray<FIntPoint> Result = Indexes;
	for (int32 i = 0; i < Result.Num(); i++)
	{
		Result[i].X += Offset.X;
		Result[i].Y += Offset.Y;
	}
	return Result;
}
