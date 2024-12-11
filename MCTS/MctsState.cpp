// Fill out your copyright notice in the Description page of Project Settings.


#include "MctsState.h"

bool UMctsState::IsPositionOccupied(FIntPoint Position, bool bCheckForEnemy, bool bCheckForTeam) const
{
	// check for team for spell like healing, check for enemy for spell like damaging
	if (bCheckForTeam)
	{
		int32 CurTeam = Units[CurUnitIndex].TeamIndex;
		for (const auto& Unit : Units)
		{
			if (Unit.Position == Position && Unit.TeamIndex == CurTeam && !Unit.bIsCrystal)
			{
				return true;
			}
		}
	}
	else if (bCheckForEnemy)
	{
		int32 CurTeam = Units[CurUnitIndex].TeamIndex;
		for (const auto& Unit : Units)
		{
			if (Unit.Position == Position && Unit.TeamIndex != CurTeam)
			{
				return true;
			}
		}
	}
	else
	{
		for (const auto& Unit : Units)
		{
			if (Unit.Position == Position)
			{
				return true;
			}
		}
	}
	return false;
}

int32 UMctsState::GetDistanceToTarget(FIntPoint StartIndex, FIntPoint TargetIndex, TArray<ETileTypeC> ValidTileType)
{
	TQueue<TPair<FIntPoint, int32>> Queue;
	TMap<FIntPoint, FIntPoint> CameFrom; // Tracks the parent of each tile
	const TArray<FIntPoint> Directions = { FIntPoint(0, 1), FIntPoint(1, 0), FIntPoint(0, -1), FIntPoint(-1, 0) };

	Queue.Enqueue(TPair<FIntPoint, int32>(StartIndex, 0));
	CameFrom.Add(StartIndex, StartIndex); // Root has no parent but points to itself

	while (!Queue.IsEmpty())
	{
		TPair<FIntPoint, int32> Current;
		Queue.Dequeue(Current);

		FIntPoint Position = Current.Key;
		int32 Dist = Current.Value;

		if (Position == TargetIndex)
		{
			// Reconstruct path from TargetIndex to StartIndex
			TArray<FIntPoint> Path;
			FIntPoint Step = TargetIndex;

			while (Step != StartIndex)
			{
				Path.Insert(Step, 0);
				Step = CameFrom[Step];
			}
			Path.Insert(StartIndex, 0); // Include the start position
			return Path.Num();
		}

		for (FIntPoint Direction : Directions)
		{
			FIntPoint Neighbor = Position + Direction;


			if (Grid->Contains(Neighbor) && ValidTileType.Contains((*Grid)[Neighbor]) && !CameFrom.Contains(Neighbor))
			{
				Queue.Enqueue(TPair<FIntPoint, int32>(Neighbor, Dist + 1));
				CameFrom.Add(Neighbor, Position); // Track how we reached this tile
			}
		}
	}

	// Return high number if the target is not reachable
	return 99;
}

TArray<FIntPoint> UMctsState::GetValidMovesIndexes(FIntPoint StartIndex, int32 MoveRange, TArray<ETileTypeC> ValidTileType)
{
	TQueue<TPair<FIntPoint, int32>> Queue;
	TArray<FIntPoint> Visited;
	const TArray<FIntPoint> Directions = { FIntPoint(0, 1), FIntPoint(1, 0), FIntPoint(0, -1), FIntPoint(-1, 0) };

	Queue.Enqueue(TPair<FIntPoint, int32>(StartIndex, 0));
	Visited.Add(StartIndex);

	while (!Queue.IsEmpty())
	{
		TPair<FIntPoint, int32> Current;
		Queue.Dequeue(Current);

		FIntPoint Position = Current.Key;
		int32 Dist = Current.Value;

		if (Dist >= MoveRange)
		{
			continue;
		}

		for (FIntPoint Direction : Directions)
		{
			FIntPoint Neighbor = Position + Direction;
			if (Grid->Contains(Neighbor) && ValidTileType.Contains((*Grid)[Neighbor]) && !Visited.Contains(Neighbor) && !IsPositionOccupied(Neighbor))
			{
				Queue.Enqueue(TPair<FIntPoint, int32>(Neighbor, Dist + 1));
				Visited.Add(Neighbor);
			}
		}
	}
	Visited.Remove(StartIndex);

	return Visited;
}


UMctsState::UMctsState()
{
}

UMctsState::~UMctsState()
{
}

void UMctsState::Init(TArray<FEntityData> U, TSharedPtr<TMap<FIntPoint, ETileTypeC>> G, FEvalData ED, int32 CUI, int32 OI, int32 OCI, int32 T, EUtilityGoals UG, bool bAT)
{
	Units = U;
	Grid = G;
	EvalData = ED;
	CurUnitIndex = CUI;
	OriUnitIndex = OI;
	OriCrystalIndex = OCI;
	Turn = T;
	Goal = UG;
	bAiTurn = bAT;
	WinningTeam = -1;
}

void UMctsState::InitStartState(TArray<FEntityData> U, int32 CUI, EUtilityGoals UG, TSharedPtr<TMap<FIntPoint, ETileTypeC>> GridPtr)
{
	Units = U;
	CurUnitIndex = CUI;
	OriUnitIndex = CUI;
	OriCrystalIndex = -1;
	Turn = 0;
	Goal = UG;
	bAiTurn = true;
	WinningTeam = -1;
	Grid = GridPtr;
	EvalData = FEvalData();
	TArray<FIntPoint> CrystalPos;
	TArray<int32> CrystalIndex;

	for (int32 i = 0; i < Units.Num(); i++)
	{
		const FEntityData& Unit = Units[i];
		if (Unit.TeamIndex == 0)
		{
			if (Unit.bIsCrystal)
			{
				EvalData.OriCrystalHealth += Unit.Health;
				EvalData.OriCrystalCount++;
				CrystalPos.Add(Unit.Position);
				CrystalIndex.Add(i);
			}
			else
			{
				EvalData.OriEnemyHealth += Unit.Health;
				EvalData.OriEnemyCount++;
			}
		}
		else if (Unit.TeamIndex == 1)
		{
			EvalData.OriAllyHealth += Unit.Health;
			EvalData.OriAllyCount++;
		}
	}
	
	if (Goal == EUtilityGoals::AttackCrystal)
	{
		int32 MinDist = 1000;
		int32 Dist = 0;
		for (int32 i = 0; i < CrystalPos.Num(); i++)
		{
			const FIntPoint Pos = CrystalPos[i];
			Dist = GetDistanceToTarget(Units[OriUnitIndex].Position, Pos, Units[OriUnitIndex].ValidTileType);
			if (Dist < MinDist) 
			{
				OriCrystalIndex = CrystalIndex[i];
				MinDist = Dist;
			}
		}
		EvalData.OriCrystalDist = MinDist;
	}
	else
	{
		int32 MinDist = 1000;
		int32 Dist = 0;
		for (const FEntityData& Unit : Units)
		{
			if (Unit.TeamIndex == 0)
			{
				Dist = GetDistanceToTarget(Units[OriUnitIndex].Position, Unit.Position, Units[OriUnitIndex].ValidTileType);
				if (Dist < MinDist)
					MinDist = Dist;
			}
		}
		EvalData.OriEnemyDist = MinDist;
	}
}

void UMctsState::ActionsToTry(TQueue<FMctsAction>& Actions)
{
	SpellUtils SpellUtility;
	TArray<FSpellData> ListSpell = Units[CurUnitIndex].Spells;
	TArray<FIntPoint> Index;
	
    for (const FSpellData& Spell : ListSpell)
	{
		if (Spell.ApCost <= Units[CurUnitIndex].Ap && Spell.CooldownLeft == 0)
		{
			Index = SpellUtility.GetSpellIndexes(Units[CurUnitIndex].Position, Spell.Range, Spell.Pattern);

			if (Spell.bIsAOE)
			{
				TArray<FIntPoint> AOEIndex;
				TArray<FIntPoint> AOETargetIndex;
				for (FIntPoint I : Index)
				{
					AOEIndex = SpellUtility.GetSpellIndexes(I, Spell.AoeRange, Spell.AoePattern);
					for (FIntPoint J : AOEIndex)
					{
						if (IsPositionOccupied(J, true))
						{
							AOETargetIndex.Add(J);
						}
					}
					// if there are enemy in the spell splash range, recheck again to get all target index including ally
					if (!AOETargetIndex.IsEmpty())
					{
						AOETargetIndex.Empty();
						for (FIntPoint J : AOEIndex)
						{
							if (IsPositionOccupied(J))
							{
								AOETargetIndex.Add(J);
							}
						}
					}
					if (!AOETargetIndex.IsEmpty())
					{
						FMctsAction Action = FMctsAction();
						Action.Init(EActionType::Spell, I, Spell, AOETargetIndex);
						Actions.Enqueue(Action);
						AOETargetIndex.Empty();
					}
				}
			}
			else 
			{
				for (FIntPoint I : Index)
				{
					if (IsPositionOccupied(I, true, !Spell.IsDamaging))
					{
						FMctsAction Action = FMctsAction();
						Action.Init(EActionType::Spell, I, Spell);
						Actions.Enqueue(Action);
					}
				}
			}
		}
	}

	// Get the valid moves for the unit and add them to the queue
	Index = GetValidMovesIndexes(Units[CurUnitIndex].Position, Units[CurUnitIndex].MovementRange, Units[CurUnitIndex].ValidTileType);
	for (FIntPoint I : Index)
	{
		FMctsAction Action = FMctsAction();
		Action.Init(EActionType::Move, I);
		Actions.Enqueue(Action);
	}

	// If there are no actions to try, add an end turn action
	if (Actions.IsEmpty())
	{
		FMctsAction Action = FMctsAction();
		Action.Init(EActionType::EndTurn,FIntPoint());
		Actions.Enqueue(Action);
	}
}

TArray<FMctsAction> UMctsState::ArrayActionsToTry()
{
	SpellUtils SpellUtility;
	TArray<FSpellData> ListSpell = Units[CurUnitIndex].Spells;
	TArray<FIntPoint> Index;
	TArray<FMctsAction> Actions;

	for (const FSpellData& Spell : ListSpell)
	{
		if (Spell.ApCost <= Units[CurUnitIndex].Ap && Spell.CooldownLeft == 0)
		{
			Index = SpellUtility.GetSpellIndexes(Units[CurUnitIndex].Position, Spell.Range, Spell.Pattern);

			if (Spell.bIsAOE)
			{
				TArray<FIntPoint> AOEIndex;
				TArray<FIntPoint> AOETargetIndex;
				for (FIntPoint I : Index)
				{
					AOEIndex = SpellUtility.GetSpellIndexes(I, Spell.AoeRange, Spell.AoePattern);
					for (FIntPoint J : AOEIndex)
					{
						if (IsPositionOccupied(J))
						{
							AOETargetIndex.Add(J);
						}
					}
					if (!AOETargetIndex.IsEmpty())
					{
						FMctsAction Action = FMctsAction();
						Action.Init(EActionType::Spell, I, Spell, AOETargetIndex);
						Actions.Add(Action);
						AOETargetIndex.Empty();
					}
				}
			}
			else
			{
				for (FIntPoint I : Index)
				{
					if (IsPositionOccupied(I, true, !Spell.IsDamaging))
					{
						FMctsAction Action = FMctsAction();
						Action.Init(EActionType::Spell, I, Spell);
						Actions.Add(Action);
					}
				}
			}
		}
	}

	// Return Attack Actions if there are any if not check for movement
	if (!Actions.IsEmpty())
		return Actions;

	// Get the valid moves for the unit and add them to the queue
	Index = GetValidMovesIndexes(Units[CurUnitIndex].Position, Units[CurUnitIndex].MovementRange, Units[CurUnitIndex].ValidTileType);
	for (FIntPoint I : Index)
	{
		FMctsAction Action = FMctsAction();
		Action.Init(EActionType::Move, I);
		Actions.Add(Action);
	}

	// If there are no actions to try, add an end turn action
	if (Actions.IsEmpty())
	{
		FMctsAction Action = FMctsAction();
		Action.Init(EActionType::EndTurn, FIntPoint());
		Actions.Add(Action);
	}

	return Actions;
}

UMctsState* UMctsState::NextState(const FMctsAction Action) const
{
	UMctsState* NewState = new UMctsState();
	NewState->Init(Units, Grid, EvalData, CurUnitIndex, OriUnitIndex, OriCrystalIndex, Turn, Goal, bAiTurn);
	SpellUtils SpellUtility;
	TArray<FEntityData> UnitsToRemove;
	TQueue<FIntPoint> ExplosionQueue;

	if (Action.ActionType == EActionType::Spell)
	{
		int32 SpellIndex = 0;
		NewState->Units[NewState->CurUnitIndex].Spells.Find(Action.Spell, SpellIndex);
		NewState->Units[NewState->CurUnitIndex].Ap -= Action.Spell.ApCost;
		NewState->Units[NewState->CurUnitIndex].Spells[SpellIndex].CooldownLeft = Action.Spell.Cooldown;

		if (Action.Spell.bIsAOE) 
		{
			for (FIntPoint I : Action.AoeTarget)
			{
				for (FEntityData& Unit : NewState->Units)
				{
					if (Unit.Position == I)
					{
						// team index 2 is barrel, so if barrel hit, trigger barrel explosion
						if (Unit.TeamIndex == 2) 
						{
							ExplosionQueue.Enqueue(Unit.Position);
						}
						Unit.ApplyDamage(Action.Spell.Damage);
						if (!Unit.IsUnitAlive())
							UnitsToRemove.Add(Unit);
					}
				}
			}
		}
		else
		{
			for (FEntityData& Unit : NewState->Units)
			{
				if (Unit.Position == Action.Target)
				{
					// team index 2 is barrel, so if barrel hit, trigger barrel explosion
					if (Unit.TeamIndex == 2)
					{
						ExplosionQueue.Enqueue(Unit.Position);
					}
					Unit.ApplyDamage(Action.Spell.Damage);
					if (!Unit.IsUnitAlive())
						UnitsToRemove.Add(Unit);
				}
			}
		}
	}
	else if (Action.ActionType == EActionType::Move)
	{
		NewState->Units[CurUnitIndex].Position = Action.Target;
		NewState->Units[CurUnitIndex].Ap -= 1;
	}

	while (!ExplosionQueue.IsEmpty())
	{
		FIntPoint ExplosionPosition;
		ExplosionQueue.Dequeue(ExplosionPosition);

		FSpellData CounterSpell = NewState->Units[CurUnitIndex].Spells[0];
		TArray<FIntPoint> ExplosionIndexes = SpellUtility.GetSpellIndexes(ExplosionPosition, CounterSpell.AoeRange, CounterSpell.AoePattern);

		for (FIntPoint ExplosionIndex : ExplosionIndexes)
		{
			for (FEntityData& Unit : NewState->Units)
			{
				if (Unit.Position == ExplosionIndex)
				{
					if (Unit.TeamIndex == 2 && Unit.Position != ExplosionPosition) // Another barrel hit
					{
						ExplosionQueue.Enqueue(Unit.Position);
					}
					Unit.ApplyDamage(CounterSpell.Damage);
					if (!Unit.IsUnitAlive())
					{
						UnitsToRemove.Add(Unit);
					}
				}
			}
		}
	}

	FEntityData OriCrystal;
	FEntityData OriUnit;
	bool bOriUnitIsDead = true;
	FEntityData CurrentUnit = NewState->Units[CurUnitIndex];
	bool bCurUnitIsDead = !CurrentUnit.IsUnitAlive();

	// Handle case where original crystal destroyed
	if (NewState->OriCrystalIndex != -1) 
	{
		if (!NewState->Units[OriCrystalIndex].IsUnitAlive()) 
			NewState->OriCrystalIndex = -1;
		else
			OriCrystal = NewState->Units[NewState->OriCrystalIndex];
	}
	
	if (NewState->OriUnitIndex != -1)
	{
		OriUnit = NewState->Units[NewState->OriUnitIndex];
		bOriUnitIsDead = !OriUnit.IsUnitAlive();
	}
	// set new current unit if current unit is dead. need to check before remove dead unit
	while (!CurrentUnit.IsUnitAlive() || CurrentUnit.TeamIndex == 2 || CurrentUnit.bIsCrystal)
	{
		int32 index = NewState->Units.IndexOfByKey(CurrentUnit);
		index = (index + 1) % NewState->Units.Num();
		CurrentUnit = NewState->Units[index];
	}

	// Remove dead units
	for (FEntityData Unit : UnitsToRemove)
	{
		NewState->Units.Remove(Unit);
	}

	if (NewState->OriCrystalIndex != -1)
	{
		NewState->OriCrystalIndex = NewState->Units.IndexOfByKey(OriCrystal);
	}

	if (bOriUnitIsDead)
	{
		NewState->OriUnitIndex = -1;
	}
	else
	{
		NewState->OriUnitIndex = NewState->Units.IndexOfByKey(OriUnit);
	}

	NewState->CurUnitIndex = NewState->Units.IndexOfByKey(CurrentUnit);

	// Resolve the turn if the current unit has no more AP and move to the next unit
	if (NewState->Units.Num() != 0)
	{
		if (bCurUnitIsDead)
		{
			NewState->Turn++;
			if (NewState->Units[NewState->CurUnitIndex].TeamIndex == 1)
				NewState->bAiTurn = true;
			else
				NewState->bAiTurn = false;
		}
		else if (NewState->Units[NewState->CurUnitIndex].Ap == 0 or Action.ActionType == EActionType::EndTurn)
		{
			NewState->Turn++;
			NewState->Units[NewState->CurUnitIndex].ResolveTurn();
			NewState->CurUnitIndex = (NewState->CurUnitIndex + 1) % NewState->Units.Num();
			while (NewState->Units[NewState->CurUnitIndex].TeamIndex == 2 || NewState->Units[NewState->CurUnitIndex].bIsCrystal)
			{
				NewState->CurUnitIndex = (NewState->CurUnitIndex + 1) % NewState->Units.Num();
			}
			if (NewState->Units[NewState->CurUnitIndex].TeamIndex == 1)
				NewState->bAiTurn = true;
			else
				NewState->bAiTurn = false;
		}
	}
	NewState->CalculateWinners();
	return NewState;
}

double UMctsState::Rollout()
{
	if (IsTerminal())
	{
		// If the game is over, return the reward
		return Evaluate();
	}

	UMctsState* SimulatedState = new UMctsState();
	SimulatedState->Init(Units, Grid, EvalData, CurUnitIndex, OriUnitIndex, OriCrystalIndex, Turn, Goal, bAiTurn);

	while (!SimulatedState->IsTerminal())
	{
		TArray<FMctsAction> PossibleActions;
		PossibleActions = SimulatedState->ArrayActionsToTry();

		if (PossibleActions.IsEmpty())
		{
			break;
		}

		// Randomly select an Action from the possible Actions
		int32 RandomIndex = FMath::RandRange(0,  PossibleActions.Num() - 1);
		FMctsAction SelectedAction = PossibleActions[RandomIndex];

		// Apply the selected Action to transition to the next state
		UMctsState* OldState = SimulatedState;
		SimulatedState = SimulatedState->NextState(SelectedAction);
		delete OldState;
	}

	// Calculate and return the reward based on the final state
	double Score = SimulatedState->Evaluate();
	delete SimulatedState;
	return Score;
}

double UMctsState::Evaluate()
{
	// If the AI lose give 0.0, if the AI win give 2.0 so it has better score than everything else
	if (WinningTeam == 0)
		return 0.0;
	else if (WinningTeam == 1)
		return 1.0;

	double Score = 0.0;
	int32 EnemyHealth = 0;
	int32 AllyHealth = 0;
	int32 HealthCrystal = 0;
	int32 EnemyCount = 0;
	int32 AllyCount = 0;
	int32 CrystalCount = 0;
	int32 MinDist = 1000;
	int32 Dist = 0;
	TArray<FIntPoint> CrystalPos;

	for (const FEntityData& Unit : Units)
	{
		if (Unit.TeamIndex == 0)
		{
			if (Unit.bIsCrystal) {
				CrystalPos.Add(Unit.Position);
				HealthCrystal += Unit.Health;
				CrystalCount++;
			}
			else 
			{
				EnemyHealth += Unit.Health;
				EnemyCount++;
			}
		}
		else if (Unit.TeamIndex == 1)
		{
			AllyHealth += Unit.Health;
			AllyCount++;
		}
	}

	if (Goal == EUtilityGoals::AttackCrystal)
	{
		// If the original crystal is destroyed, set distance to 0
		if (OriCrystalIndex == -1)
			MinDist = 0;
		// If the original unit is still alive, calculate the distance between the original unit and the crystal
		else if (OriUnitIndex != -1)
		{
			for (const auto Pos : CrystalPos)
			{
				Dist = GetDistanceToTarget(Units[OriUnitIndex].Position, Pos, Units[OriUnitIndex].ValidTileType);
				if (Dist < MinDist)
					MinDist = Dist;
			}
		}
		else // If the original unit is dead, set distance to max so score will be 0
			MinDist = EvalData.OriCrystalDist;
	}
	else
	{
		if (OriUnitIndex != -1)
		{
			for (const FEntityData& Unit : Units)
			{
				if (Unit.TeamIndex == 0 && !Unit.bIsCrystal)
				{
					Dist = GetDistanceToTarget(Units[OriUnitIndex].Position, Unit.Position, Units[OriUnitIndex].ValidTileType);
					if (Dist < MinDist)
						MinDist = Dist;
				}
			}
		}
		else
			MinDist = EvalData.OriEnemyDist;
	}

	double CrystalDmgScore = (double)EvalData.OriCrystalHealth - HealthCrystal;
	double Team1DmgScore = (double)EvalData.OriAllyHealth - AllyHealth;
	double Team0DmgScore = (double)EvalData.OriEnemyHealth - EnemyHealth;
	double CrystalScore = (double)EvalData.OriCrystalCount - CrystalCount;
	double KillScore = (double)EvalData.OriEnemyCount - EnemyCount;
	double DeadScore = (double)EvalData.OriAllyCount - AllyCount;
	double CrystalDistScore = (double)EvalData.OriCrystalDist - MinDist;
	double EnemyDistScore = (double)EvalData.OriEnemyDist - MinDist;

	switch (Goal)
	{
	case EUtilityGoals::AttackCrystal:
		if (EvalData.OriCrystalDist <= 3)
			Score =
				CrystalDmgScore * 0.02 +
				Team1DmgScore * - 0.01 +
				Team0DmgScore * 0.02 +
				CrystalScore * 0.4 +
				KillScore * 0.3 +
				DeadScore * -0.15;
		else
			Score = 
				CrystalDmgScore * 0.02 +
				Team1DmgScore * -0.01 +
				Team0DmgScore * 0.02 +
				CrystalScore * 0.4 +
				KillScore * 0.3 +
				DeadScore * -0.15 +
				CrystalDistScore * 0.02;
		break;
	case EUtilityGoals::Retreat:
		Score = 
			CrystalDmgScore * 0.015 +
			Team1DmgScore * - 0.02 +
			Team0DmgScore * 0.01 +
			CrystalScore * 0.4 +
			KillScore * 0.3 +
			DeadScore * -0.3 +
			EnemyDistScore * -0.04; //negative so it have higher score when farther from enemy
		break;
	case EUtilityGoals::KillingSpree:
		Score = 
			CrystalDmgScore * 0.015 +
			Team1DmgScore * - 0.015 +
			Team0DmgScore * 0.03 +
			CrystalScore * 0.3 +
			KillScore * 0.5 +
			DeadScore * -0.15 +
			EnemyDistScore * 0.02;
		break;
	default:
		break;
	}

	Score = FMath::Clamp(Score, 0.0, 1.0);

	return Score;
}

void UMctsState::CalculateWinners()
{
	int32 EnemyCount = 0;
	int32 AllyCount = 0;
	int32 CrystalCount = 0;

	for (const FEntityData& Unit : Units)
	{
		if (Unit.TeamIndex == 0)
		{
			if (Unit.bIsCrystal)
				CrystalCount++;
			else
				EnemyCount++;
		}
		else if (Unit.TeamIndex == 1)
			AllyCount++;
	}

	if (EnemyCount == 0 || CrystalCount == 0)
		WinningTeam = 1;
	else if (AllyCount == 0)
		WinningTeam = 0;
}

bool UMctsState::IsTerminal() const
{
	//set default turn limit to 5, could change later
	if (Turn >= 5 || WinningTeam != -1)
		return true;

	return false;
}

void UMctsState::Print() const
{
}

bool UMctsState::IsAiTurn() const
{
	return bAiTurn;
}

