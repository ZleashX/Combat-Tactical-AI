// Fill out your copyright notice in the Description page of Project Settings.


#include "MctsNode.h"
#include "Async/Async.h"
#include "HAL/RunnableThread.h"

#define DEBUG

void UMctsNode::Backpropagate(double W, int N)
{
	Score += W;
	NumOfSim += N;

	if (Parent != NULL) {
		Parent->Backpropagate(W, N);
	}
}

UMctsNode::UMctsNode()
	: NumOfSim(0), Score(0.0), State(nullptr), Action(FMctsAction()), Parent(nullptr)
{
}

UMctsNode::~UMctsNode()
{
	if (State != nullptr)
		delete State;
	for (auto* Child : Children)
	{
		delete Child;
	}
	Children.Empty();
}

void UMctsNode::Init(UMctsNode* P, UMctsState* S, const FMctsAction A)
{
	Parent = P;
	State = S;
	Action = A;
	Children = TArray<UMctsNode*>();
	State->ActionsToTry(UntriedActions);
	bTerminal = State->IsTerminal();
}

bool UMctsNode::IsFullyExpanded() const
{
	return IsTerminal() || UntriedActions.IsEmpty();
}

bool UMctsNode::IsTerminal() const
{
	return bTerminal;
}

const FMctsAction UMctsNode::GetAction() const
{
	return Action;
}

void UMctsNode::Expand()
{
	if (bTerminal)
	{
		Rollout();
		return;
	}
	else if (IsFullyExpanded())
	{
		UE_LOG(LogTemp, Warning, TEXT("Warning: Cannot expanded this node any more"));
		return;
	}

	FMctsAction NextAction = FMctsAction();
	UntriedActions.Dequeue(NextAction);
	UMctsState* NextState = State->NextState(NextAction);

	UMctsNode* NewNode = new UMctsNode();
	NewNode->Init(this, NextState, NextAction);
	NewNode->Rollout();
	Children.Add(NewNode);
}

void UMctsNode::Rollout()
{
	const int32 NumParallelRollouts = 6;

	// Store future results from parallel rollouts
	TArray<TFuture<double>> RolloutResults;

	// Launch each rollout in parallel
	for (int32 i = 0; i < NumParallelRollouts; ++i)
	{
		RolloutResults.Add(Async(EAsyncExecution::TaskGraph, [this]()
			{
				return State->Rollout();
			}));
	}

	// Wait for all rollouts to complete and gather results
	double MaxScore = -DBL_MAX;

	for (const TFuture<double>& Result : RolloutResults)
	{
		double W = Result.Get();
		if (W > MaxScore)
		{
			MaxScore = W;
		}
	}

	// Call Backpropagate with the aggregated score
	Backpropagate(MaxScore, 1);
}

UMctsNode* UMctsNode::SelectBestChild(double C) const
{
	if (Children.IsEmpty()) return nullptr;
	else if (Children.Num() == 1) return Children[0];
	else
	{
		double Uct, Max = -DBL_MAX;
		UMctsNode* Argmax = nullptr;
		for (auto* Child : Children) 
		{
			double Winrate = Child->Score / ((double) Child->NumOfSim);
			if (!State->IsAiTurn()) 
			{
				Winrate = 1 - Winrate;
			}
			if (C > 0) 
			{
				Uct = Winrate + C * sqrt(log((double) this->NumOfSim) / ((double) Child->NumOfSim));
			}
			else
			{
				Uct = Winrate;
#ifdef DEBUG
				if (Child->Action.ActionType == EActionType::Move)
					UE_LOG(LogTemp, Display, TEXT("Action : (%d,%d)"), Child->Action.Target.X, Child->Action.Target.Y);
				if (Child->Action.ActionType == EActionType::Spell)
					UE_LOG(LogTemp, Display, TEXT("Action : %s"), *Child->Action.Spell.Name);
				UE_LOG(LogTemp, Display, TEXT("Score : %f"), (float)Winrate);
				if (Child->Action.ActionType == EActionType::Spell)
				{
					for (FEntityData& Unit : State->Units)
					{
						if (Unit.bIsCrystal)
						{
							UE_LOG(LogTemp, Display, TEXT("Before Crystal Health : %d"), Unit.Health);
						}
					}
					for (FEntityData& Unit : Child->State->Units)
					{
						if (Unit.bIsCrystal)
						{
							UE_LOG(LogTemp, Display, TEXT("After Crystal Health : %d"), Unit.Health);
						}
					}
				}
#endif
			}
			if (Uct > Max) 
			{
				Max = Uct;
				Argmax = Child;
			}
		}
#ifdef DEBUG
		if (C == 0)
			UE_LOG(LogTemp, Display, TEXT("Best Child Score : %f"), (float)Max);
#endif
		if (Argmax == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Warning: No best child found!"));
			return Children[0];
		}
		return Argmax;
	}
}

UMctsNode* UMctsNode::AdvanceTree(const FMctsAction A)
{
	UMctsNode* Next = nullptr;
	for (auto* Child : Children)
	{
		if (Child->Action==A)
			Next = Child;
		else
			delete Child;
	}
	Children.Empty();

	if (Next == nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("INFO: Didn't find child node. Had to start over."));
		UMctsState* NextState = State->NextState(A);
		Next = new UMctsNode();
		Next->Init(this, NextState, A);
	}
	else 
	{
		Next->Parent = nullptr;
	}
	return Next;
}

const UMctsState* UMctsNode::GetCurrentState() const
{
	return State;
}

void UMctsNode::PrintStats() const //need to implement
{
}

