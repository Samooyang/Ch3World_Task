#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SpartaGameStateBase.generated.h"

UCLASS()
class CH3WORLD_API ASpartaGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	ASpartaGameStateBase();
	
	
	virtual void BeginPlay() override;
	
	//Score
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 Score;
	
	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScore() const;
	
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Amount);
	
	
	//Coin
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 SpawnedCoinCount;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 CollectedCoinCount;
	
	void OnCoinCollected();
	
	//Level
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	float LevelDuration;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 MaxLevels;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TArray<FName> LevelMapNames;
	
	UFUNCTION(BlueprintCallable, Category = "Level")
	void OnGameOver();
	
	void StartLevel();
	void OnLevelTimeUp();
	void EndLevel();
	
	//Timer
	FTimerHandle LevelTimerHandle;
	FTimerHandle HUDUpdateTimerHandle;
	
	//widget
	void UpdateHUD();
};
