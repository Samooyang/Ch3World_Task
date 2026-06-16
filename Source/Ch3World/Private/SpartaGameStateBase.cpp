#include "SpartaGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "SpartaPlayerController.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "SpartaGameInstance.h"
#include "Components/ProgressBar.h"
#include "SpartaCharacter.h"

ASpartaGameStateBase::ASpartaGameStateBase()
{
	//Score
	Score = 0;
	
	//Coin
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	
	//Level & Waves
	MaxLevels = 3;
	
	CurrentWaveIndex = 0;
}

void ASpartaGameStateBase::BeginPlay()
{
	Super::BeginPlay();
	StartLevel();
	
	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle,
		this,
		&ASpartaGameStateBase::UpdateHUD,
		0.1f,
		true
		);
}

int32 ASpartaGameStateBase::GetScore() const
{
	return Score;
}

void ASpartaGameStateBase::AddScore(int32 Amount)
{
	Score += Amount;
	
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance))
		{
			SpartaGameInstance->AddToScore(Amount);
		}
	}
}

void ASpartaGameStateBase::StartLevel()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->ShowGameHUD();
		}
	}
	CurrentWaveIndex = 0;
	StartWave();
}
	
void ASpartaGameStateBase::StartWave()
{
	if (Waves.IsValidIndex(CurrentWaveIndex))
	{
		FWaveInfo& CurrentWave = Waves[CurrentWaveIndex];
		
		GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Red, FString::Printf(TEXT("Wave %d Start!!!"), CurrentWaveIndex + 1));
	
		SpawnedCoinCount = 0;
		CollectedCoinCount = 0;
		
		TArray<AActor*> FoundVolumes;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);
		
		if (FoundVolumes.Num() > 0)
		{
			int32 RandomIndex = FMath::RandRange(0, FoundVolumes.Num() - 1);
			ASpawnVolume* SelectedVolume = Cast<ASpawnVolume>(FoundVolumes[RandomIndex]);
				
			if (SelectedVolume)
			{
				for (int32 i = 0; i < CurrentWave.SpawnCount; i++)
				{
					AActor* SpawnedActor = SelectedVolume->SpawnRandomItem();
					
					if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
					{
						SpawnedCoinCount++;
					}
				}
			}
		}
		
		
		GetWorldTimerManager().SetTimer(
		LevelTimerHandle,
		this,
		&ASpartaGameStateBase::OnWaveTimeUp,
		CurrentWave.WaveDuration,
		false
		);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Red, TEXT("Waves All Clear!!!"));
		EndLevel();
	}
}	
	
void ASpartaGameStateBase::OnWaveTimeUp()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue,TEXT("Times Up!!! Game Over!!!"));
	OnGameOver();
}

void ASpartaGameStateBase::OnCoinCollected()
{
	CollectedCoinCount++;

	if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
	{
		GetWorldTimerManager().ClearTimer(LevelTimerHandle);
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange,FString::Printf(TEXT("Wave %d Clear!!!"), CurrentWaveIndex + 1));
		
		CurrentWaveIndex++;
		StartWave();
	}
}

void ASpartaGameStateBase::EndLevel()
{
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);
	
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance))
		{
			SpartaGameInstance->CurrentLevelIndex++;
			
			if (SpartaGameInstance->CurrentLevelIndex >= MaxLevels)
			{
				OnGameOver();
				return;
			}
			if (LevelMapNames.IsValidIndex(SpartaGameInstance->CurrentLevelIndex))
			{
				UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[SpartaGameInstance->CurrentLevelIndex]);
			}
			else
			{
				OnGameOver();
			}
		}
	}
}

void ASpartaGameStateBase::OnGameOver()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->SetPause(true);
			SpartaPlayerController->ShowMainMenu(true);
		}
	}
	
}

void ASpartaGameStateBase::UpdateHUD()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
			{
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("TimeText"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
				}
				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("ScoreText"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
						if (SpartaGameInstance)
						{
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), SpartaGameInstance->TotalScore)));
						}
					}
				}
				if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("LevelIndexText"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance))
						{
							LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), SpartaGameInstance->CurrentLevelIndex + 1)));
						}
					}
				}
				if (UTextBlock* WaveText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("WaveText"))))
				{
					WaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave: %d"), CurrentWaveIndex + 1)));
				}
				
				if (ASpartaCharacter* SpartaCharacter = Cast<ASpartaCharacter>(PlayerController->GetPawn()))
				{
					if (UProgressBar* HPBar = Cast<UProgressBar>(HUDWidget->GetWidgetFromName(TEXT("HPBar"))))
					{
						float HealthPercent = SpartaCharacter->GetHealth() / SpartaCharacter->GetMaxHealth();
						HPBar->SetPercent(HealthPercent);
					}
					if (UTextBlock* HPText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("HPText"))))
					{
						HPText->SetText(FText::FromString(
							FString::Printf(TEXT("%.0f / %.0f"), SpartaCharacter->GetHealth(), SpartaCharacter->GetMaxHealth())));
					}
				}
			}
		}
	}
}