#include "OBExtractPoint.h"
#include "Components/CapsuleComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBExtractPoint)

AOBExtractPoint::AOBExtractPoint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Capsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Capsule->SetLineThickness(2.f);
	Capsule->ShapeColor = FColor::Green;

	Capsule->OnComponentBeginOverlap.AddDynamic(this, &AOBDeviceBase::OnOverlapBegin);
	Capsule->OnComponentEndOverlap.AddDynamic(this, &AOBDeviceBase::OnOverlapEnd);

	SegmentsToProgressMap.Add(1, 0.f);
	SegmentsToProgressMap.Add(2, 0.f);
	SegmentsToProgressMap.Add(3, 0.f);

	CurrentSegment = 1;

	SecondsEachSegment = 20;
	SecondsSegmentFall = 40;

	bHasProgress = true;

	DeviceName = "Extract";
}

void AOBExtractPoint::BeginPlay()
{
	Super::BeginPlay();

	RegisterDevice(this);
}

void AOBExtractPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

//
// Debug Messages
//
	GEngine->AddOnScreenDebugMessage(00, 0.1f, FColor::White, FString::Printf(TEXT("Progress: %f"), CurrentProgress));
	GEngine->AddOnScreenDebugMessage(11, 0.1f, FColor::Orange, FString::Printf(TEXT("Segment: %i"), CurrentSegment));
	GEngine->AddOnScreenDebugMessage(22, 0.1f, FColor::Yellow, FString::Printf(TEXT("CharactersInCapsule: %i"), CharactersInCapsule.Num()));
	GEngine->AddOnScreenDebugMessage(33, 0.1f, FColor::Emerald, FString::Printf(TEXT("PlayersInCapsule: %i"), CapsulePlayers.Num()));
	GEngine->AddOnScreenDebugMessage(44, 0.1f, FColor::Emerald, FString::Printf(TEXT("EnemysInCapsule: %i"), CapsuleEnemys.Num()));
	GEngine->AddOnScreenDebugMessage(55, 0.1f, FColor::Green, bIsActive ? FString::Printf(TEXT("IsActive: true")) : FString::Printf(TEXT("IsActive: false")));

	if (CharactersInCapsule.Num() == 0)
	{
		bIsActive = false;
		if (CurrentProgress > 0.f)
			CurrentProgress = FMath::Max(0.f, CurrentProgress - (DeltaTime / SecondsSegmentFall));
	}
	else
	{
		if(!bIsActive)
			bIsActive = true;
	}

	if (bIsActive)
	{
		if (CapsuleEnemys.Num() == 0 && CapsulePlayers.Num() > 0)
			CurrentProgress = FMath::Min(1.f, CurrentProgress + (DeltaTime / SecondsEachSegment));

		SegmentsToProgressMap.Emplace(CurrentSegment, CurrentProgress);
	}

	if (CurrentProgress >= 1.f)
	{
		if (CurrentSegment < 3)
		{
			CurrentSegment++;
			CurrentProgress = 0.f;
		}
		else if (CurrentSegment == 3)
		{
			SetActorTickEnabled(false);
			bIsActive = false;

			Capsule->SetLineThickness(0.25f);
			Capsule->ShapeColor = FColor::Red;
			Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			Capsule->OnComponentBeginOverlap.Clear();
			Capsule->OnComponentEndOverlap.Clear();

			OnCompletedDelegate.Broadcast();
		}
	}
}

void AOBExtractPoint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
