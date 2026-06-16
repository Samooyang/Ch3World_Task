#include "CoinItem.h"
#include "Engine/World.h"
#include "SpartaGameStateBase.h"

ACoinItem::ACoinItem()
{
	PointValue = 0;
	ItemType = "DefaultCoin";
}

void ACoinItem::ActivateItem(AActor* Activator)
{
	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (UWorld* World = GetWorld())
		{
			if (ASpartaGameStateBase* GameState = World->GetGameState<ASpartaGameStateBase>())
			{
				//test용 텍스트
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, 
					FString::Printf(TEXT("Player gained %d points!"), PointValue));
				
				GameState->AddScore(PointValue);
				GameState->OnCoinCollected();
			}
		}
		DestroyItem();
	}
}