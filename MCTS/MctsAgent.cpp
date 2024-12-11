// Fill out your copyright notice in the Description page of Project Settings.


#include "MctsAgent.h"
#include "Async/Async.h"

UMctsAgent::UMctsAgent()
{
}

void UMctsAgent::Init(TMap<FIntPoint, ETileTypeC> GameGrid, int32 Mi, int32 Ms)
{
	this->Grid = MakeShared<TMap<FIntPoint, ETileTypeC>>(GameGrid);
	this->MaxIter = Mi;
	this->MaxSec = Ms;
}

UMctsAgent::~UMctsAgent()
{
}

const TArray<FMctsAction> UMctsAgent::GenAction(TArray<FEntityData> U, int32 CUI, EUtilityGoals UG)
{
	UMctsState* StartingState = new UMctsState();
	StartingState->InitStartState(U, CUI, UG, Grid);
	Tree = new UMctsTree();
	Tree->Init(StartingState);
	Tree->GrowTree(MaxIter, MaxSec);
	TArray<FMctsAction> BestActions;

	UMctsNode* BestChild = Tree->SelectBestChild();
	if (BestChild == NULL) {
		UE_LOG(LogTemp, Warning, TEXT("Warning: Tree root has no children! Possibly terminal node!"));
		return BestActions;
	}

	BestActions.Add(BestChild->GetAction());
	Tree->AdvanceTree(BestActions[0]);
	BestChild = Tree->SelectBestChild();
	if (BestChild == NULL) {
		UE_LOG(LogTemp, Warning, TEXT("Warning: Tree root has no children! Possibly terminal node!"));
		return BestActions;
	}
	BestActions.Add(BestChild->GetAction());
	delete Tree;
	return BestActions;
}

void UMctsAgent::StartGenActionAsync(TArray<FEntityData> U, int32 CUI, EUtilityGoals UG)
{
	TWeakObjectPtr<UMctsAgent> WeakThis = this;

	// Run GenAction in a background thread
	Async(EAsyncExecution::TaskGraph, [WeakThis, U, CUI, UG]()
		{
			if (WeakThis.IsValid())
			{
				TArray<FMctsAction> BestActions = WeakThis->GenAction(U, CUI, UG);

				// Dispatch the result back to the main thread to trigger Blueprint event
				Async(EAsyncExecution::TaskGraphMainThread, [WeakThis, BestActions]()
					{
						if (WeakThis.IsValid())
						{
							WeakThis->OnGenActionCompleted(BestActions); // Trigger the Blueprint event
						}
					});
			}
		});
}

const UMctsState* UMctsAgent::GetCurrentState() const
{
	return Tree->GetCurrentState();
}

void UMctsAgent::Feedback() const
{
	Tree->PrintStats();
}
