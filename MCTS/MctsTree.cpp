// Fill out your copyright notice in the Description page of Project Settings.


#include "MctsTree.h"

#define DEBUG

UMctsTree::UMctsTree()
{
	Root = nullptr;
}

UMctsTree::~UMctsTree()
{
	delete Root;
}

void UMctsTree::Init(UMctsState* StartingState)
{
	Root = new UMctsNode();
	Root->Init(nullptr, StartingState, FMctsAction());
}

UMctsNode* UMctsTree::Select(double C)
{
	UMctsNode* Node = Root;
	while (!Node->IsTerminal())
	{
		if (!Node->IsFullyExpanded())
			return Node;
		else 
		{
			Node = Node->SelectBestChild(C);
		}
	}
	return Node;
}

UMctsNode* UMctsTree::SelectBestChild()
{
	return Root->SelectBestChild(0.0);
}

void UMctsTree::GrowTree(int32 MaxIter, double MaxTimeInSec)
{
	UMctsNode* Node;
	double dt;
#ifdef DEBUG
	UE_LOG(LogTemp, Display, TEXT("Growing tree...."));
#endif
	time_t start_t, now_t;
	time(&start_t);
	for (int i = 0; i < MaxIter; i++)
	{
		Node = Select();
		Node->Expand();
		time(&now_t);
		dt = difftime(now_t, start_t);
		if (dt >= MaxTimeInSec) 
		{
#ifdef DEBUG
			UE_LOG(LogTemp, Display, TEXT("Early stopping: Made %d iterations in %f seconds."), (i + 1), dt);
#endif
			break;
		}
	}
#ifdef DEBUG
	time(&now_t);
	dt = difftime(now_t, start_t);
	UE_LOG(LogTemp, Display, TEXT("Finished growing tree in %f seconds."), dt);
#endif
}

void UMctsTree::AdvanceTree(const FMctsAction Move)
{
	UMctsNode* OldRoot = Root;
	Root = Root->AdvanceTree(Move);
	delete OldRoot;
}

const UMctsState* UMctsTree::GetCurrentState() const
{
	return Root->GetCurrentState();
}

void UMctsTree::PrintStats() const
{
	Root->PrintStats();
}
