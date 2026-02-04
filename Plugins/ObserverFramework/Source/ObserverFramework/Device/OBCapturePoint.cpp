#include "OBCapturePoint.h"
#include "OBDeviceComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameStateBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBCapturePoint)

AOBCapturePoint::AOBCapturePoint()
{
	PrimaryActorTick.TickInterval = 0.1f;
	PrimaryActorTick.bCanEverTick = true;
}

void AOBCapturePoint::BeginPlay()
{
	Super::BeginPlay();
	SetActiveAndRegister();
}

void AOBCapturePoint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ResetCaptureProgress();
	Super::EndPlay(EndPlayReason);
}

void AOBCapturePoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetLocalRole() == ROLE_Authority && IsActivated)
	{
		UpdateNearbyPlayers();
		UpdateCaptureProgress();
	}
}

void AOBCapturePoint::SetActiveAndRegister()
{
	UWorld* World = GetWorld();
	if (World && GetLocalRole() == ROLE_Authority)
	{
		AGameStateBase* GameState = World->GetGameState<AGameStateBase>();
		if (GameState)
		{
			UOBDeviceComponent* DeviceManager = GameState->FindComponentByClass<UOBDeviceComponent>();
			if (DeviceManager)
			{
				DeviceManager->ActiveCapturePoints.Add(this);
				IsActivated = true;}
		}
	}
}

void AOBCapturePoint::UpdateNearbyPlayers()
{
	TeamAllyCount = 0;
	TeamEnemyCount = 0;

	TArray<FHitResult> OverlapResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	FVector CenterLocation = GetActorLocation();
	GetWorld()->SweepMultiByChannel(OverlapResults, CenterLocation, CenterLocation,
		FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(CaptureRadius), QueryParams);

	for (const FHitResult& Result : OverlapResults)
	{
		ACharacter* Character = Cast<ACharacter>(Result.GetActor());
		if (Character && Character->GetController())
		{
			TeamAllyCount++;
		}
	}
}

void AOBCapturePoint::UpdateCaptureProgress()
{
	if (TeamAllyCount == 0 && TeamEnemyCount == 0)
	{
		CurrentState = ECapturePointState::Neutral;
		ResetCaptureProgress();
		return;
	}

	if (TeamEnemyCount > 0)
	{
		CurrentState = ECapturePointState::Contested;
		return;
	}

	if (TeamAllyCount > 0)
	{
		CurrentState = ECapturePointState::Capturing;
		float ProgressIncrement = (1.0f / CaptureTime) * (TeamAllyCount > 1 ? 1.2f : 1.0f);
		CurrentCaptureProgress += ProgressIncrement;

		if (CurrentCaptureProgress >= 1.0f)
		{
			CurrentCaptureProgress = 1.0f;
			CurrentState = ECapturePointState::Captured;
			OnCaptureStateChanged.Broadcast(CurrentState, nullptr);
		}
	}
}

void AOBCapturePoint::ResetCaptureProgress()
{
	CurrentCaptureProgress = 0.0f;
}

void AOBCapturePoint::Server_SetCaptureState_Implementation(ECapturePointState NewState, AController* CapturingController)
{
	CurrentState = NewState;
	OnCaptureStateChanged.Broadcast(NewState, CapturingController);
}

bool AOBCapturePoint::Server_SetCaptureState_Validate(ECapturePointState NewState, AController* CapturingController)
{
	return true;
}
