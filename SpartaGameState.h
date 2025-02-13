// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SpartaGameState.generated.h"

class AQuestion;

UCLASS()
class PROJECT_6_1_API ASpartaGameState : public AGameState
{
	GENERATED_BODY()
public:
	ASpartaGameState();

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32	Score;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Coin")
	int32	SpawnedCoinCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Coin")
	int32	CollectedCoinCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	float LevelDuration;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Level")
	int32	CurrentLevelIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Level")
	int32 MaxLevel;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TArray<FName> LevelMapNames;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	TArray<AQuestion*> QuestionItemArrList;

	FTimerHandle LevelTimerHandle;
	FTimerHandle TimeTimerHandle;

	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScore() const;
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Amount);
	UFUNCTION(BlueprintCallable, Category = "Level")
	void OnGameOver();

	void UpdateQuestionItem(AQuestion* actor);
	void StartLevel();
	void OnLevelTimeUp();
	void OnGameCollected();
	void EndLevel();
	void UpdateHUD();
private:

};
