// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MctsState.h"
#include "Containers/Queue.h"

class UMctsNode
{
private:

	bool bTerminal;
	int32 NumOfSim;
	double Score;
	UMctsState *State;
	FMctsAction Action;
	TArray<UMctsNode*> Children;
	UMctsNode* Parent;

	TQueue<FMctsAction> UntriedActions;
	void Backpropagate(double W, int N);
public:
	UMctsNode();
	~UMctsNode();
	void Init(UMctsNode* Parent, UMctsState* State, const FMctsAction Action);
	bool IsFullyExpanded() const;
	bool IsTerminal() const;
	const FMctsAction GetAction() const;
	void Expand();
	void Rollout();
	UMctsNode* SelectBestChild(double C) const;
	UMctsNode* AdvanceTree(const FMctsAction A);
	const UMctsState* GetCurrentState() const;
	void PrintStats() const;
};
