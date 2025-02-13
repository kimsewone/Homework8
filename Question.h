// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "project_6_1/BaseItem.h"
#include "Question.generated.h"

UENUM(BlueprintType)
enum class EQItemType : uint8 
{
	Coin = 0, 
	Mine = 1
};


UCLASS()
class PROJECT_6_1_API AQuestion : public ABaseItem
{
	GENERATED_BODY()
public:
	AQuestion();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 ExplosionDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 CoinValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	TArray<UParticleSystem*> RandParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	TArray<USoundBase*> RandSound;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Item|SubObject")
	TSubclassOf<ABaseItem> SubItemClass;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Item|SubObject")
	int32 RandItem;

	UFUNCTION(BlueprintCallable,Category = "Item|SubObject")
	void SetItem(EQItemType ItemIdx);

protected:
	virtual void ActivateItem(AActor* Activator) override;
	virtual void BeginPlay() override; //BeginPlay 만들면 Destroy()안된다. 아직 이유 못찾음..20250213
	virtual void Tick(float DeltaTime) override;
private:

	FTimerHandle QuestionItemRotationTimerHandle;
	FTimerHandle RandomResultTimerHandle;
	bool bHasResult;
	
	float QuestionItemTimeElapsed;
	float RandTimeElapsed;

	void Explode(AActor* Activator);

};
