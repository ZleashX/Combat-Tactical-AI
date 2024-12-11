// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MctsTree.h"
#include "MctsAgent.generated.h"

/**
 * 
 */
UCLASS(BlueprintType,Blueprintable)
class TACTICSAI_API UMctsAgent : public UObject
{
	GENERATED_BODY()

	UMctsTree* Tree;

	UPROPERTY()
	int32 MaxIter;

	UPROPERTY()
	int32 MaxSec;
	

	TSharedPtr<TMap<FIntPoint, ETileTypeC>> Grid;
public:
	UMctsAgent();

	UFUNCTION(BlueprintCallable)
	void Init(TMap<FIntPoint, ETileTypeC> GameGrid, int32 MaxIteration = 100000, int32 MaxSeconds = 2);

	~UMctsAgent();

	UFUNCTION(BlueprintCallable)
	const TArray<FMctsAction> GenAction(TArray<FEntityData> U, int32 CUI, EUtilityGoals UG);

	UFUNCTION(BlueprintCallable)
	void StartGenActionAsync(TArray<FEntityData> U, int32 CUI, EUtilityGoals UG);

	UFUNCTION(BlueprintImplementableEvent)
	void OnGenActionCompleted(const TArray<FMctsAction>& BestActions);

	const UMctsState* GetCurrentState() const;
	void Feedback() const;
};
