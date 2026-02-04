#include "OBLootPoint.h"
#include "GameFramework/Character.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBLootPoint)

AOBLootPoint::AOBLootPoint()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f;
	bReplicates = true;

	Tags.Add(FName("Loot"));
}

void AOBLootPoint::BeginPlay()
{
	Super::BeginPlay();

	RegisterDevice(this);
	CurrentState = ELootPointState::Locked;
}

void AOBLootPoint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ResetUnlock();
	Super::EndPlay(EndPlayReason);
}

void AOBLootPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetLocalRole() == ROLE_Authority && CurrentState == ELootPointState::Unlocking)
	{
		UpdateUnlockProgress(DeltaTime);
	}
}

void AOBLootPoint::StartUnlock(AController* KeyHolder)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		Server_StartUnlock(KeyHolder);
	}
	else
	{
		Server_StartUnlock(KeyHolder);
	}
}

void AOBLootPoint::Server_StartUnlock_Implementation(AController* KeyHolder)
{
	if (CurrentState != ELootPointState::Locked || !HasKeyRequired(KeyHolder))
	{
		return;
	}

	CurrentState = ELootPointState::Unlocking;
	CurrentUnlocker = KeyHolder;
	CurrentUnlockProgress = 0.0f;
	OnLootStateChanged.Broadcast(CurrentState, KeyHolder);
	Multicast_PlayAlertSound();
}

bool AOBLootPoint::Server_StartUnlock_Validate(AController* KeyHolder)
{
	return KeyHolder != nullptr;
}

void AOBLootPoint::CancelUnlock()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		Server_CancelUnlock();
	}
	else
	{
		Server_CancelUnlock();
	}
}

void AOBLootPoint::Server_CancelUnlock_Implementation()
{
	if (CurrentState == ELootPointState::Unlocking)
	{
		ResetUnlock();
		CurrentState = ELootPointState::Locked;
		OnLootStateChanged.Broadcast(CurrentState, nullptr);
	}
}

bool AOBLootPoint::Server_CancelUnlock_Validate()
{
	return true;
}

void AOBLootPoint::UpdateUnlockProgress(float DeltaTime)
{
	if (CurrentUnlocker.IsValid())
	{
		CurrentUnlockProgress += DeltaTime / UnlockDuration;

		if (CurrentUnlockProgress >= 1.0f)
		{
			CompleteUnlock();
		}
	}
}

void AOBLootPoint::CompleteUnlock()
{
	CurrentUnlockProgress = 1.0f;
	CurrentState = ELootPointState::Unlocked;
	OnLootStateChanged.Broadcast(CurrentState, CurrentUnlocker.Get());
}

void AOBLootPoint::ResetUnlock()
{
	CurrentUnlockProgress = 0.0f;
	CurrentUnlocker = nullptr;
}

bool AOBLootPoint::HasKeyRequired(AController* KeyHolder) const
{
	// Vereinfachte Logik: Müsste mit echtem Inventory-System verbunden werden
	if (!KeyHolder)
	{
		return false;
	}

	ACharacter* Character = Cast<ACharacter>(KeyHolder->GetCharacter());
	if (!Character)
	{
		return false;
	}

	// Beispiel: Prüfung auf Item-Tag oder Inventory-Component
	return true;
}

void AOBLootPoint::Multicast_PlayAlertSound_Implementation()
{
	// Audio-Cue für Alert-Sound abspielen
	// USoundCue* AlertSound = LoadObject<USoundCue>(nullptr, TEXT("..."));
	// UGameplayStatics::PlaySoundAtLocation(GetWorld(), AlertSound, GetActorLocation());
}
