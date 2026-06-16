#include "SpartaGameMode.h"
#include "SpartaCharacter.h"
#include "SpartaPlayerController.h"
#include "SpartaGameStateBase.h"

ASpartaGameMode::ASpartaGameMode()
{
	DefaultPawnClass = ASpartaCharacter::StaticClass();
	
	PlayerControllerClass = ASpartaPlayerController::StaticClass();
	
	GameStateClass = ASpartaGameStateBase::StaticClass();
}
