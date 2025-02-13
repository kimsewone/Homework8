// Fill out your copyright notice in the Description page of Project Settings.


#include "SubCoin.h"

// Sets default values
ASubCoin::ASubCoin()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//EnableTick(false);
	MotionState = 0;
	CoinSpeed = 250.f;
}

void ASubCoin::ActivateItem(AActor* Activator)
{
}

// Called when the game starts or when spawned
void ASubCoin::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASubCoin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (MotionState == 0)
	{
		CoinSpeed += DeltaTime * 1000.f;
		if (CoinSpeed >= 600.f)
		{
			MotionState = 1;
		}
	}
	else if (MotionState == 1)
	{
		CoinSpeed -= DeltaTime * 1000.f;
		if (CoinSpeed <= -600.f)
		{
			MotionState = 2;
		}
	}
	else 
	{
		DestroyItem();
	}
	StaticMesh->AddRelativeLocation(FVector::UpVector * DeltaTime * CoinSpeed*0.6f);
	StaticMesh->AddRelativeRotation(FRotator(0.0f,1.f,0.f) * DeltaTime * 400.f);
	

}



