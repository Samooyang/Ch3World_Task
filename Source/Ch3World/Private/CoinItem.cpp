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
				GameState->AddScore(PointValue);
				GameState->OnCoinCollected();
			}
		}
	}
	Super::ActivateItem(Activator);
}