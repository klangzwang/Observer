#include "OBPlayerController.h"
#include "OBLocalPlayer.h"
#include "Components/ControllerComponent.h"
#include "Components/GameFrameworkComponentManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBPlayerController)

AOBPlayerController::AOBPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AOBPlayerController::ReceivedPlayer()
{
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);

	Super::ReceivedPlayer();

	TArray<UControllerComponent*> ModularComponents;
	GetComponents(ModularComponents);
	for (UControllerComponent* Component : ModularComponents)
	{
		Component->ReceivedPlayer();
	}

	if (UOBLocalPlayer* LocalPlayer = Cast<UOBLocalPlayer>(Player))
	{
		LocalPlayer->OnPlayerControllerSet.Broadcast(LocalPlayer, this);

		if (PlayerState)
		{
			LocalPlayer->OnPlayerStateSet.Broadcast(LocalPlayer, PlayerState);
		}
	}
}

void AOBPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	if (UOBLocalPlayer* LocalPlayer = Cast<UOBLocalPlayer>(Player))
	{
		LocalPlayer->OnPlayerPawnSet.Broadcast(LocalPlayer, InPawn);
	}
}

void AOBPlayerController::OnPossess(APawn* APawn)
{
	Super::OnPossess(APawn);

	if (UOBLocalPlayer* LocalPlayer = Cast<UOBLocalPlayer>(Player))
	{
		LocalPlayer->OnPlayerPawnSet.Broadcast(LocalPlayer, APawn);
	}

/*
	if (!CurrentGameLayout && !DefaultGameLayoutClass.IsNull())
	{
		TSubclassOf<UOBPrimaryGameLayout> GameLayoutClass = DefaultGameLayoutClass.LoadSynchronous();
		if (ensure(GameLayoutClass && !GameLayoutClass->HasAnyClassFlags(CLASS_Abstract)))
		{
			CurrentGameLayout = CreateWidget<UOBPrimaryGameLayout>(this, GameLayoutClass);
			CurrentGameLayout->AddToPlayerScreen(1000);
		}
	}
*/
}

void AOBPlayerController::OnUnPossess()
{
	Super::OnUnPossess();

	if (UOBLocalPlayer* LocalPlayer = Cast<UOBLocalPlayer>(Player))
	{
		LocalPlayer->OnPlayerPawnSet.Broadcast(LocalPlayer, nullptr);
	}

	//CurrentGameLayout = nullptr;
}

void AOBPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (PlayerState)
	{
		if (UOBLocalPlayer* LocalPlayer = Cast<UOBLocalPlayer>(Player))
		{
			LocalPlayer->OnPlayerStateSet.Broadcast(LocalPlayer, PlayerState);
		}
	}
}

void AOBPlayerController::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void AOBPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);
}

void AOBPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	TArray<UControllerComponent*> ModularComponents;
	GetComponents(ModularComponents);
	for (UControllerComponent* Component : ModularComponents)
	{
		Component->PlayerTick(DeltaTime);
	}
}
