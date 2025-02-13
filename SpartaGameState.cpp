// Fill out your copyright notice in the Description page of Project Settings.


#include "SpartaGameState.h"
#include "Kismet/GameplayStatics.h"
#include "project_6_1/SpawnVolume.h"
#include "project_6_1/CoinItem.h"
#include "SpartaGameInstance.h"
#include "project_6_1/SpartaPlayerController.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "Question.h"

ASpartaGameState::ASpartaGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	LevelDuration = 30.f;
	CurrentLevelIndex = 0;
	MaxLevel = 3;
}

void ASpartaGameState::BeginPlay()
{
	Super::BeginPlay();
	StartLevel();

	GetWorldTimerManager().SetTimer(
		TimeTimerHandle,
		this,
		&ASpartaGameState::UpdateHUD,
		0.1f,
		true
	);
}

int32 ASpartaGameState::GetScore() const
{
	return Score;
}

void ASpartaGameState::AddScore(int32 Amount)
{
	if (UGameInstance* instance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(instance);
		SpartaGameInstance->AddToScore(Amount);
	}
//	Score += Amount;
//	UE_LOG(LogTemp, Warning, TEXT("CurScore: %d / GetPoint: %d"), Score, Amount);
}



void ASpartaGameState::UpdateQuestionItem(AQuestion* Actor)
{
	QuestionItemArrList.Remove( Actor );
}

void ASpartaGameState::StartLevel()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->ShowGameHUD();
		}
	}

	if (UGameInstance* instance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(instance);
		CurrentLevelIndex = SpartaGameInstance->GetCurrentLevelIndex();
	}

	int32 TempQuestionItemCnt = 0;
	QuestionItemArrList.Empty();

	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass( GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	const int32 ItemToSpawn = 40;

	for (int32 i = 0; i < ItemToSpawn;++i)
	{
		if (FoundVolumes.Num() > 0)
		{
			ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
			if (SpawnVolume)
			{
				AActor* SpawnActor = SpawnVolume->SpawnRandomItem();
				if (SpawnActor && SpawnActor->IsA(ACoinItem::StaticClass()))
				{
					SpawnedCoinCount++;
				}
				else if (SpawnActor && SpawnActor->IsA(AQuestion::StaticClass()))
				{
					//SpawnedCoinCount++;
					QuestionItemArrList.Add(Cast<AQuestion>(SpawnActor) );
					TempQuestionItemCnt++;
				}
			}
		}
	}
	//한 라운드안에 나오는 일반 Coin최하 갯수 -> Question아이템 최대갯수는 SpawnedCoinCount보다 많으면 안된다.  
	TempQuestionItemCnt = SpawnedCoinCount < TempQuestionItemCnt ? SpawnedCoinCount : TempQuestionItemCnt;

	//적당한 랜덤값으로 CoinItem 타입으로 정해준다.
	int32 CoinItemCount = TempQuestionItemCnt - FMath::RandRange(0, TempQuestionItemCnt) ;

	//Question이라는 아이템은 물음표 형상의 아이템이고 습득하면 coin이나 폭탄이 나온다..
	for(int32 i = 0; i < CoinItemCount;++i)
	{
		AQuestion* QItem = QuestionItemArrList[i];

		if (QItem ) { 
			QItem->SetItem(EQItemType::Coin);//Coin Item
			SpawnedCoinCount--;
		}
	}
	// CoinItem 타입으로 뿌려주고 남은 Question Item들은 폭탄으로 만든다.  
	for (int32 i = CoinItemCount; i < QuestionItemArrList.Num();++i)
	{
		QuestionItemArrList[i]->SetItem(EQItemType::Mine);//Mine Item
	}
	UE_LOG(LogTemp, Warning, TEXT("SpawnedCoinCnt:%d /QItemList Total:%d / CoinItemCnt:%d / QMineCnt:%d"), SpawnedCoinCount, QuestionItemArrList.Num(), CoinItemCount, QuestionItemArrList.Num() - CoinItemCount);
	
	GetWorldTimerManager().SetTimer(
		LevelTimerHandle,
		this,
		&ASpartaGameState::OnLevelTimeUp,
		LevelDuration,
		false
	);
	//UE_LOG(LogTemp, Warning, TEXT("Level %d Start!, Spawned %d coin "), CurrentLevelIndex+1, SpawnedCoinCount);
}

void ASpartaGameState::OnLevelTimeUp()
{
	EndLevel();
}

void ASpartaGameState::EndLevel()
{
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);
	
	USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GetWorld()->GetGameInstance());
	if (SpartaGameInstance)
	{
		//AddScore(Score);
		CurrentLevelIndex++;
		SpartaGameInstance->SetCurrentLevelIndex(CurrentLevelIndex);
	}

	if (CurrentLevelIndex > MaxLevel)
	{
		OnGameOver();
		return;
	}
	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		//GetWorldTimerManager().ClearAllTimersForObject(this);
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else
	{
		OnGameOver();
	}
}
void ASpartaGameState::UpdateHUD()
{
	ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(GetWorld()->GetFirstPlayerController());
	if (SpartaPlayerController)
	{
		if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
		{
			if (UTextBlock* TimeText = Cast<UTextBlock>( HUDWidget->GetWidgetFromName(TEXT("Time") )))
			{
				float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
				TimeText->SetText( FText::FromString( FString::Printf(TEXT("Time: %.1f"), RemainingTime) ) );
			}
			if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
			{
				UGameInstance* GameInstance = GetGameInstance();
				if (GameInstance)
				{
					USpartaGameInstance* SpartaGameInstance = Cast< USpartaGameInstance>(GameInstance);
					if(SpartaGameInstance )
					{
						ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), SpartaGameInstance->TotalScore)));
					}
				}
			}
			if (UTextBlock* LevelText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
			{
				LevelText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), CurrentLevelIndex+1)));
			}

			if (UTextBlock* QuestionText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Question"))))
			{
				int CurQuestionMireCnt = 0;
				for (int i = 0; i < QuestionItemArrList.Num();++i)
				{
					if (IsValid(QuestionItemArrList[i]))
					{
						if( QuestionItemArrList[i]->RandItem == (int)EQItemType::Mine)
							CurQuestionMireCnt++;
					}
				}
				QuestionText->SetText(FText::FromString(FString::Printf(TEXT("Question Item 지뢰 : %d개 "), CurQuestionMireCnt)));
			}
		}
	}
}
void ASpartaGameState::OnGameOver()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->ShowMainMenu(true);
			SpartaPlayerController->SetPause(true);
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("Game Over!"));
}

void ASpartaGameState::OnGameCollected()
{
	CollectedCoinCount++;
	UE_LOG(LogTemp, Warning, TEXT("Coin Collected: %d / %d"),CollectedCoinCount,SpawnedCoinCount );

	if (CollectedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
	{
		EndLevel();
	}
}
