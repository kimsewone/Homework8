// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "project_6_1/BaseItem.h"
#include "SubCoin.generated.h"

UCLASS()
class PROJECT_6_1_API ASubCoin : public ABaseItem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASubCoin();

	int32 MotionState;
	float CoinSpeed;
	
	virtual void ActivateItem(AActor* Activator) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

};
