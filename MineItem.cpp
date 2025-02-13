// Fill out your copyright notice in the Description page of Project Settings.


#include "MineItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AMineItem::AMineItem()
{
	ExplosionDelay = 2.5f;
	ExplosionRadius = 400.f;
	ExplosionDamage = 30;
	ItemType = "Mine";

	ExplosionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollision"));
	ExplosionCollision->InitSphereRadius(ExplosionRadius);
	ExplosionCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	ExplosionCollision->SetupAttachment(Scene);

	bHasExploded = false;
}

void AMineItem::ActivateItem(AActor* Activator)
{
	if (bHasExploded) return;

	Super::ActivateItem(Activator);
	GetWorld()->GetTimerManager().SetTimer(ExplosionTimerHandle, this,&AMineItem::Explode ,ExplosionDelay, false);
	//DestroyItem();
	bHasExploded = true;
}

void AMineItem::Explode()
{
	TArray<AActor*> OverlappingActors;
	ExplosionCollision->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (Actor && Actor->ActorHasTag("Player"))
		{
			UGameplayStatics::ApplyDamage(
				Actor, 
				ExplosionDamage,
				nullptr,//유발자인데 현재는 null
				this,
				UDamageType::StaticClass()//UDamageType타입의 기본적인 클래스
			);
			//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Player Damage %d "), ExplosionDamage));
		}
	}

	UParticleSystemComponent* Particle = nullptr;;
	if (ExplosionParticle)
	{
		Particle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ExplosionParticle,
			GetActorLocation(),
			GetActorRotation(),
			false
		);
	}
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			ExplosionSound,
			GetActorLocation()
		);
	}
	
	if (Particle)
	{
		TWeakObjectPtr<ABaseItem> WeakThis(this);
		FTimerHandle DestroyParticleTimerHandle;

		GetWorld()->GetTimerManager().SetTimer(
			DestroyParticleTimerHandle,
			[this,Particle, WeakThis]()
			{
				if (IsValid(Particle)&& WeakThis.IsValid())
				{
					Particle->DestroyComponent();//Deactivate();
					//StaticMesh->SetHiddenInGame(true);//SetVisibility(false);
					//this->DestroyItem();
					UE_LOG(LogTemp, Warning, TEXT("Particle!!"));
				}
			},
			2.f,
			false
		);
	}
	DestroyItem();
}
