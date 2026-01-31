#include "OBGameMode.h"
#include "OBGameState.h"
#include "OBPlayerController.h"
#include "OBPlayerState.h"
#include "OBGameSession.h"
#include "OBHUD.h"
#include "OBCharacter.h"
#include "CommonActivatableWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBGameMode)

AOBGameModeBase::AOBGameModeBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

AOBGameMode::AOBGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	HUDLayout = FOBHUDLayoutRequest();
	GameSessionClass = AOBGameSession::StaticClass();
	GameStateClass = AOBGameState::StaticClass();
	PlayerControllerClass = AOBPlayerController::StaticClass();
	PlayerStateClass = AOBPlayerState::StaticClass();
	DefaultPawnClass = AOBCharacter::StaticClass();
	HUDClass = AOBHUD::StaticClass();
}

void AOBGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
}

void AOBGameMode::InitGameState()
{
	Super::InitGameState();
}
