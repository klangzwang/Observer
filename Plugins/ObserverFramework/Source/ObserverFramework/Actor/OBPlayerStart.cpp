#include "OBPlayerStart.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "TimerManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBPlayerStart)

AOBPlayerStart::AOBPlayerStart(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

EOBPlayerStartLocationOccupancy AOBPlayerStart::GetLocationOccupancy(AController* const ControllerPawnToFit) const
{
	UWorld* const World = GetWorld();
	if (HasAuthority() && World)
	{
		if (AGameModeBase* AuthGameMode = World->GetAuthGameMode())
		{
			TSubclassOf<APawn> PawnClass = AuthGameMode->GetDefaultPawnClassForController(ControllerPawnToFit);
			const APawn* const PawnToFit = PawnClass ? GetDefault<APawn>(PawnClass) : nullptr;

			FVector ActorLocation = GetActorLocation();
			const FRotator ActorRotation = GetActorRotation();

			if (!World->EncroachingBlockingGeometry(PawnToFit, ActorLocation, ActorRotation, nullptr))
			{
				return EOBPlayerStartLocationOccupancy::Empty;
			}
			else if (World->FindTeleportSpot(PawnToFit, ActorLocation, ActorRotation))
			{
				return EOBPlayerStartLocationOccupancy::Partial;
			}
		}
	}

	return EOBPlayerStartLocationOccupancy::Full;
}

bool AOBPlayerStart::IsClaimed() const
{
	return ClaimingController != nullptr;
}

bool AOBPlayerStart::TryClaim(AController* OccupyingController)
{
	if (OccupyingController != nullptr && !IsClaimed())
	{
		ClaimingController = OccupyingController;
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(ExpirationTimerHandle, FTimerDelegate::CreateUObject(this, &AOBPlayerStart::CheckUnclaimed), ExpirationCheckInterval, true);
		}
		return true;
	}
	return false;
}

void AOBPlayerStart::CheckUnclaimed()
{
	if (ClaimingController != nullptr && ClaimingController->GetPawn() != nullptr && GetLocationOccupancy(ClaimingController) == EOBPlayerStartLocationOccupancy::Empty)
	{
		ClaimingController = nullptr;
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(ExpirationTimerHandle);
		}
	}
}

