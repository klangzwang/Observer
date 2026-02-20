#include "OBCapturePoint.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PlayerStart.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBCapturePoint)

AOBCapturePoint::AOBCapturePoint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Capsule->OnComponentBeginOverlap.AddDynamic(this, &AOBDeviceBase::OnOverlapBegin);
	Capsule->OnComponentEndOverlap.AddDynamic(this, &AOBDeviceBase::OnOverlapEnd);

	SecondsToCapture = 10;
	SecondsCaptureFall = 5;

	bIsCapturedByPlayer = false;
	bIsCapturedByEnemy = true;

	DeviceName = "Capture";
}

void AOBCapturePoint::BeginPlay()
{
	Super::BeginPlay();

	RegisterDevice(this);
	SetActorTickEnabled(true);

	if (GetWorld())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 100.f);
		FRotator SpawnRotation = GetActorRotation();

		RespawnPlayerStart = GetWorld()->SpawnActor<APlayerStart>(APlayerStart::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);
		
		RespawnPlayerStart->SetActorHiddenInGame(false);
	}
}

void AOBCapturePoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//
	// Debug Messages
	//
	/*
	GEngine->AddOnScreenDebugMessage(00, 0.1f, FColor::White, FString::Printf(TEXT("Progress: %f"), CurrentProgress));
	GEngine->AddOnScreenDebugMessage(11, 0.1f, FColor::Yellow, FString::Printf(TEXT("CharactersInCapsule: %i"), CharactersInCapsule.Num()));
	GEngine->AddOnScreenDebugMessage(22, 0.1f, FColor::Emerald, FString::Printf(TEXT("PlayersInCapsule: %i"), CapsulePlayers.Num()));
	GEngine->AddOnScreenDebugMessage(33, 0.1f, FColor::Emerald, FString::Printf(TEXT("EnemysInCapsule: %i"), CapsuleEnemys.Num()));
	GEngine->AddOnScreenDebugMessage(44, 0.1f, FColor::Green, bIsCapturedByPlayer ? FString::Printf(TEXT("IsCapturedByPlayer: true")) : FString::Printf(TEXT("IsCapturedByPlayer: false")));
	GEngine->AddOnScreenDebugMessage(55, 0.1f, FColor::Green, bIsCapturedByEnemy ? FString::Printf(TEXT("IsCapturedByEnemy: true")) : FString::Printf(TEXT("IsCapturedByEnemy: false")));
	GEngine->AddOnScreenDebugMessage(66, 0.1f, FColor::Yellow, FString::Printf(TEXT("CollisionProfileName: %s"), *Capsule->GetCollisionProfileName().ToString()));
	*/

	//
	//
	//
	bIsCapturedByPlayer = CurrentProgress >= 1.f ? true : false;
	bIsCapturedByEnemy = CurrentProgress <= 0.f ? true : false;

	if (!RespawnPlayerStart)
		return;

	if (bIsCapturedByPlayer)
	{
		Capsule->SetLineThickness(2.f);
		Capsule->ShapeColor = FColor::Green;
		Capsule->SetCollisionProfileName(TEXT("OverlapEnemy"));
		RespawnPlayerStart->PlayerStartTag = FName("RespawnActive");
		DeviceName = "Captured";
	}
	else if (bIsCapturedByEnemy)
	{
		Capsule->SetLineThickness(2.f);
		Capsule->ShapeColor = FColor::Red;
		Capsule->SetCollisionProfileName(TEXT("OverlapPlayer"));
		RespawnPlayerStart->PlayerStartTag = FName("");
		DeviceName = "Capture";
	}
	else
	{
		Capsule->SetLineThickness(0.25f);
		Capsule->ShapeColor = FColor::Yellow;
		Capsule->SetCollisionProfileName(TEXT("OverlapPlayerAndEnemy"));
		RespawnPlayerStart->PlayerStartTag = FName("");
		DeviceName = "Capture";
	}

	//
	//
	//
	if (CapsulePlayers.Num() == 0 && CapsuleEnemys.Num() == 0)
	{
		if(!bIsCapturedByPlayer)
			CurrentProgress = FMath::Max(0.f, CurrentProgress - (DeltaTime / SecondsCaptureFall));
	}
	else
	{
		if (CapsulePlayers.Num() > 0 && CapsuleEnemys.Num() == 0)
		{
			CurrentProgress = FMath::Min(1.f, CurrentProgress + (DeltaTime / SecondsToCapture));
		}
		else
		{
			CurrentProgress = FMath::Max(0.f, CurrentProgress - (DeltaTime / SecondsCaptureFall));
		}
	}
}

void AOBCapturePoint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
