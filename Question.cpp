// Fill out your copyright notice in the Description page of Project Settings.


#include "Question.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "project_6_1/SpartaCharacter.h"
#include "SpartaGameState.h"
#include "SubCoin.h"

AQuestion::AQuestion()
{
	PrimaryActorTick.bCanEverTick = true;
	RandTimeElapsed = 0.f;
	QuestionItemTimeElapsed = 0.f;
	ExplosionDamage = 30;
	CoinValue = 20;
	ItemType = "QuestionItem";
	QuestionItemRotationTimerHandle;
	bHasResult = false;
}

void AQuestion::BeginPlay()
{
	Super::BeginPlay();

	StaticMesh->AddRelativeRotation(FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f));
}
void AQuestion::Tick(float DeltaTime)
{
	StaticMesh->AddRelativeRotation(FRotator(0.f,1.f,0.f) * DeltaTime * 100.f);
}
void AQuestion::SetItem(EQItemType ItemIdx)
{
	RandItem = (int)ItemIdx;
}

void AQuestion::ActivateItem(AActor* Activator)
{
	if (bHasResult) return;

	Super::ActivateItem(Activator);

	bHasResult = true;

	
	if (RandItem == (int)EQItemType::Coin)
	{
		if (Activator && Activator->ActorHasTag("Player"))
		{
			if (UWorld* World = GetWorld())
			{
				ASubCoin* SpawnSubCoinActor = Cast< ASubCoin>(World->SpawnActor<ABaseItem>(SubItemClass, GetActorLocation(), FRotator::ZeroRotator));
				if (SpawnSubCoinActor)
					GetWorldTimerManager().ClearTimer(QuestionItemRotationTimerHandle);

				if (ASpartaGameState* GameState = World->GetGameState <ASpartaGameState>())
				{
					GameState->AddScore(CoinValue);
					GameState->OnGameCollected();
				}
			}
		}

		if (RandSound[0])
		{
			UGameplayStatics::PlaySoundAtLocation(
				GetWorld(),
				RandSound[0],
				GetActorLocation()
			);
		}
		DestroyItem();
		//GetWorld()->GetTimerManager().SetTimer(RandomResultTimerHandle, this, &AQuestion::RandItemSelect, 0.02f, true);
	}
	else //EQItemType::Mine
		Explode(Activator);
	
}



void AQuestion::Explode(AActor* Activator)
{
	UParticleSystemComponent* Particle = nullptr;;
	if (RandParticle[1])
	{
		Particle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			RandParticle[1],
			GetActorLocation(),
			GetActorRotation(),
			false
		);
	}
	if (RandSound[1])
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			RandSound[1],
			GetActorLocation()
		);
	}

	if (Activator && Activator->ActorHasTag("Player"))
	{
		UGameplayStatics::ApplyDamage(
			Activator,
			ExplosionDamage,
			nullptr,//유발자인데 현재는 null
			this,
			UDamageType::StaticClass()//UDamageType타입의 기본적인 클래스
		);
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Player Damage %d "), ExplosionDamage));
	}
	if (UWorld* World = GetWorld())
	{
		if (ASpartaGameState* GameState = World->GetGameState <ASpartaGameState>())
		{
			GameState->UpdateQuestionItem(this);
		}
	}

	if (Particle)
	{
		FTimerHandle DestroyParticleTimerHandle;
		TWeakObjectPtr<ABaseItem> WeakThis(this);
		GetWorld()->GetTimerManager().SetTimer(
			DestroyParticleTimerHandle,
			[this, Particle, WeakThis]()
			{
				if (IsValid(Particle)&& WeakThis.IsValid() )
				{
					Particle->DestroyComponent();//Deactivate();
					UE_LOG(LogTemp, Warning, TEXT("Particle!!"));
				}
			},
			2.f,
			false
		);
	}
	DestroyItem();
}
