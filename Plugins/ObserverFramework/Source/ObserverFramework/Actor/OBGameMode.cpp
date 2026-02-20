#include "OBGameMode.h"
#include "OBGameState.h"
#include "OBPlayerController.h"
#include "OBPlayerState.h"
#include "OBGameSession.h"
#include "OBHUD.h"
#include "OBCharacter.h"
#include "CommonActivatableWidget.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

#include "OBGameState.h"
#include "OBDeviceComponent.h"
#include "OBDeviceBase.h"

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

void AOBGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (!IsBattleMode)
		return;

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn)
	{
		PlayerPawn->OnDestroyed.AddDynamic(this, &AOBGameMode::OnPawnDestroyed);
	}
}

AActor* AOBGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		APlayerStart* Explorer = *It;
		if (Explorer && Explorer->PlayerStartTag == FName("RespawnActive"))
		{
			return Explorer;
		}
	}
	return Super::ChoosePlayerStart_Implementation(Player);
}

void AOBGameMode::OnPawnDestroyed(AActor* DestroyedActor)
{
	UOBDeviceComponent* DeviceComponent = UOBDeviceComponent::FindDeviceComponent(Cast<AOBGameState>(GetWorld()->GetGameState()));
	if (AOBDeviceBase* Device = DeviceComponent->GetAnyCapturedDeviceActive())
	{
		if (AOBCharacter* RespawnedCharacter = GetWorld()->SpawnActor<AOBCharacter>(DefaultPawnClass, Device->GetActorTransform()))
		{
			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			if (PlayerController)
			{
				PlayerController->Possess(RespawnedCharacter);
			}
		}
	}
}
