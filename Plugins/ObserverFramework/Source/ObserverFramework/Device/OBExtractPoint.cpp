#include "OBExtractPoint.h"
#include "GameFramework/Character.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBExtractPoint)

AOBExtractPoint::AOBExtractPoint()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f;
	bReplicates = true;

	Tags.Add(FName("Extract"));
}

void AOBExtractPoint::BeginPlay()
{
	Super::BeginPlay();
	
	RegisterDevice(this);
	CurrentState = EExtractPointState::Idle;
}

void AOBExtractPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetLocalRole() == ROLE_Authority && CurrentState == EExtractPointState::Extracting)
	{
		UpdateExtractionProgress(DeltaTime);
		UpdateKIAggression();
	}
}

void AOBExtractPoint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ResetExtraction();
	Super::EndPlay(EndPlayReason);
}

void AOBExtractPoint::StartExtraction()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		Server_StartExtraction();
	}
	else
	{
		Server_StartExtraction();
	}
}

void AOBExtractPoint::Server_StartExtraction_Implementation()
{
	if (CurrentState == EExtractPointState::Idle)
	{
		CurrentState = EExtractPointState::Extracting;
		CurrentExtractionProgress = 0.0f;
		CurrentSegment = 0;
		KIAggressionMultiplier = 1.0f;
		bLootPlaced = true;
		OnExtractStateChanged.Broadcast(CurrentState, 0.0f);
		Multicast_PlayExtractionSound();
	}
}

bool AOBExtractPoint::Server_StartExtraction_Validate()
{
	return true;
}

void AOBExtractPoint::InterruptExtraction()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		Server_InterruptExtraction();
	}
	else
	{
		Server_InterruptExtraction();
	}
}

void AOBExtractPoint::Server_InterruptExtraction_Implementation()
{
	if (CurrentState == EExtractPointState::Extracting)
	{
		CurrentState = EExtractPointState::Interrupted;
		OnExtractStateChanged.Broadcast(CurrentState, CurrentExtractionProgress);
	}
}

bool AOBExtractPoint::Server_InterruptExtraction_Validate()
{
	return true;
}

void AOBExtractPoint::UpdateExtractionProgress(float DeltaTime)
{
	CurrentExtractionProgress += DeltaTime / (SegmentDuration * TotalSegments);

	int32 NewSegment = FMath::FloorToInt(CurrentExtractionProgress * TotalSegments);
	if (NewSegment > CurrentSegment && NewSegment <= TotalSegments)
	{
		CurrentSegment = NewSegment;
		OnExtractSegmentComplete.Broadcast(CurrentSegment);
	}

	OnExtractStateChanged.Broadcast(CurrentState, CurrentExtractionProgress);

	if (CurrentExtractionProgress >= 1.0f)
	{
		CompleteExtraction();
	}
}

void AOBExtractPoint::UpdateKIAggression()
{
	// Eskalation: KI wird aggressiver mit jedem Segment
	float SegmentProgress = FMath::Clamp(CurrentExtractionProgress, 0.0f, 1.0f);
	KIAggressionMultiplier = 1.0f + (SegmentProgress * 2.0f);

	// Ab Segment 5 (>70%) Boss alarmieren
	if (CurrentSegment >= 5 && CurrentSegment < 6)
	{
		Multicast_AlertBoss();
	}
}

void AOBExtractPoint::CompleteExtraction()
{
	CurrentExtractionProgress = 1.0f;
	CurrentState = EExtractPointState::Completed;
	OnExtractStateChanged.Broadcast(CurrentState, 1.0f);
}

void AOBExtractPoint::ResetExtraction()
{
	CurrentExtractionProgress = 0.0f;
	CurrentSegment = 0;
	KIAggressionMultiplier = 1.0f;
	bLootPlaced = false;
}

void AOBExtractPoint::Multicast_PlayExtractionSound_Implementation()
{
	// Extraktions-Start-Sound abspielen
	// USoundCue* ExtractSound = LoadObject<USoundCue>(nullptr, TEXT("..."));
	// UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExtractSound, GetActorLocation());
}

void AOBExtractPoint::Multicast_AlertBoss_Implementation()
{
	OnBossAlerted.Broadcast();
	// Boss-Alarm-Sound und visuelle Effekte
	// USoundCue* BossAlertSound = LoadObject<USoundCue>(nullptr, TEXT("..."));
	// UGameplayStatics::PlaySoundAtLocation(GetWorld(), BossAlertSound, GetActorLocation());
}
