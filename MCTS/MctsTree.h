// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MctsNode.h"
#include "MctsState.h"

/**
 * 
 */

class UMctsTree
{
	UMctsNode* Root;
public:
	UMctsTree();
	~UMctsTree();
	void Init(UMctsState* StartingState);
	UMctsNode* Select(double C = 1.41);
	UMctsNode* SelectBestChild();
	void GrowTree(int32 MaxIter, double MaxTimeInSec);
	void AdvanceTree(const FMctsAction Action);
	const UMctsState* GetCurrentState() const;
	void PrintStats() const;
};
