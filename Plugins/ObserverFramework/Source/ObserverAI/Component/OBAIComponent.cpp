#include "OBAIComponent.h"
#include "OBAIPatrolPath.h"
#include "OBAIStorageComponent.h"
#include "AIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AIPerceptionSystem.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "GameFramework/CharacterMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBAIComponent)

UOBAIComponent::UOBAIComponent()
{
	//PrimaryComponentTick.bCanEverTick = false;
	//PrimaryComponentTick.bStartWithTickEnabled = false;

	AIOwner = nullptr;
	AIController = nullptr;
	AIContextActor = nullptr;
	AIPerception = nullptr;

	/*
	FActorTrigger OnSightTrigger;
	FActorTrigger LoseSightTrigger;
	FActorDistanceTrigger WithinDistanceTrigger;
	FActorDistanceTrigger BeyondDistanceTrigger;

	//
	// Idle
	//
	Idle.BehaviorType = EBehaviorType::Idle;
	Idle.MovementSpeed = 100.f;

	OnSightTrigger.ActorTag = "Player";
	OnSightTrigger.BehaviorTo = EBehaviorType::Follow;
	Idle.OnSightTriggers.Add(OnSightTrigger);
	OnSightTrigger.Clear();

	//
	// Follow
	//
	Follow.BehaviorType = EBehaviorType::Follow;
	Follow.MovementSpeed = 200.f;

	LoseSightTrigger.ActorTag = "Player";
	LoseSightTrigger.BehaviorTo = EBehaviorType::Idle;
	Follow.LoseSightTriggers.Add(LoseSightTrigger);
	LoseSightTrigger.Clear();

	WithinDistanceTrigger.ActorTag = "Player";
	WithinDistanceTrigger.BehaviorTo = EBehaviorType::AttackMelee;
	WithinDistanceTrigger.Distance = 100.f;
	Follow.WithinDistanceTriggers.Add(WithinDistanceTrigger);
	WithinDistanceTrigger.Clear();
	*/

	/*
	BeyondDistanceTrigger.ActorTag = "Player";
	BeyondDistanceTrigger.BehaviorTo = EBehaviorType::Investigate;
	BeyondDistanceTrigger.Distance = 900.f;
	Follow.BeyondDistanceTriggers.Add(BeyondDistanceTrigger);
	BeyondDistanceTrigger.Clear();
	*/

	/*
	//
	// Patrol
	//
	Patrol.BehaviorType = EBehaviorType::Patrol;
	Patrol.MovementSpeed = 100.f;

	OnSightTrigger.ActorTag = "Player";
	OnSightTrigger.BehaviorTo = EBehaviorType::Follow;
	Patrol.OnSightTriggers.Add(OnSightTrigger);
	OnSightTrigger.Clear();

	//
	// AttackMelee
	//
	AttackMelee.BehaviorType = EBehaviorType::AttackMelee;
	AttackMelee.MovementSpeed = 0.f;

	LoseSightTrigger.ActorTag = "Player";
	LoseSightTrigger.BehaviorTo = EBehaviorType::Idle;
	AttackMelee.LoseSightTriggers.Add(LoseSightTrigger);
	LoseSightTrigger.Clear();

	BeyondDistanceTrigger.ActorTag = "Player";
	BeyondDistanceTrigger.BehaviorTo = EBehaviorType::Follow;
	BeyondDistanceTrigger.Distance = 100.f;
	AttackMelee.BeyondDistanceTriggers.Add(BeyondDistanceTrigger);
	BeyondDistanceTrigger.Clear();

	//
	// Investigate
	//
	Investigate.BehaviorType = EBehaviorType::Investigate;
	Investigate.MovementSpeed = 50.f;

	LoseSightTrigger.ActorTag = "Player";
	LoseSightTrigger.BehaviorTo = EBehaviorType::Idle;
	Investigate.LoseSightTriggers.Add(LoseSightTrigger);
	LoseSightTrigger.Clear();

	WithinDistanceTrigger.ActorTag = "Player";
	WithinDistanceTrigger.BehaviorTo = EBehaviorType::Follow;
	WithinDistanceTrigger.Distance = 200.f;
	Investigate.WithinDistanceTriggers.Add(WithinDistanceTrigger);
	WithinDistanceTrigger.Clear();
	*/
}

void UOBAIComponent::BeginPlay()
{
	Super::BeginPlay();

	BehaviorArray.Add(Idle);
	BehaviorArray.Add(Follow);
	BehaviorArray.Add(Patrol);
	BehaviorArray.Add(AttackMelee);
	BehaviorArray.Add(Investigate);

	AIOwner = Cast<ACharacter>(GetOwner());
	if (!AIOwner)
		return;

	AIController = UAIBlueprintHelperLibrary::GetAIController(AIOwner);
	if (!AIController)
		return;

	AIPerception = AIController->GetPerceptionComponent();
	if (!AIPerception)
		return;

	FActorSpawnParameters SpawnInfo;
	AIContextActor = GetWorld()->SpawnActor<ATargetPoint>(GetOwner()->GetActorLocation(), FRotator(), SpawnInfo);

	GetAIStorageActors().AddUnique(AIOwner);
	AGameStateBase* GameState = GetOwner<AGameStateBase>();
	if (GameState)
	{
		for (APlayerState* PlayerState : GameState->PlayerArray)
		{
			APawn* PlayerPawn = PlayerState->GetPawn();
			GetAIStorageActors().AddUnique(PlayerPawn);
		}
	}

	if (!BehaviorTree)
	{
		UE_LOG(LogTemp, Warning, TEXT("This AACEnemyCharacter should be assigned with a behavior Tree, %s"), *AIOwner->GetName());
		return;
	}

	AIController->RunBehaviorTree(BehaviorTree);

	ChangeBehavior(InitialBehaviorType);

	SetTargetLocation(AIOwner->GetActorLocation());

	AIController->GetBlackboardComponent()->SetValueAsVector("StartLocation", AIOwner->GetActorLocation());
	AIController->GetBlackboardComponent()->SetValueAsObject("ContextActor", AIContextActor);
	AIController->GetBlackboardComponent()->SetValueAsObject("PatrolPath", AIPatrolPath);
	
	AIController->GetBlackboardComponent()->SetValueAsEnum("IdleArt", (uint8)CurrentBehaviorConfig.BehaviorIdleArt.IdleArt);
	AIController->GetBlackboardComponent()->SetValueAsEnum("FollowArt", (uint8)CurrentBehaviorConfig.BehaviorFollowArt.FollowArt);
	AIController->GetBlackboardComponent()->SetValueAsEnum("PatrolArt", (uint8)CurrentBehaviorConfig.BehaviorPatrolArt.PatrolArt);
}

void UOBAIComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AIContextActor != nullptr)
		AIContextActor->Destroy();
}

//
// GETTERS
//
FVector UOBAIComponent::GetTargetLocation()
{
	if (!AIController)
		return FVector();
	return AIController->GetBlackboardComponent()->GetValueAsVector("TargetLocation");
}

FVector UOBAIComponent::GetTargetLastLocation()
{
	if (!AIController)
		return FVector();
	return AIController->GetBlackboardComponent()->GetValueAsVector("TargetLastLocation");
}

AActor* UOBAIComponent::GetEQSContextActor()
{
	return AIContextActor;
}

AActor* UOBAIComponent::GetTargetActor()
{
	AActor* Actor = Cast<AActor>(AIController->GetBlackboardComponent()->GetValueAsObject("TargetActor"));
	return Actor != nullptr ? Actor : nullptr;
}

FBehaviorBasic& UOBAIComponent::GetCurrentBehaviorConfig()
{
	return CurrentBehaviorConfig;
}

ACharacter* UOBAIComponent::GetOwningCharacter()
{
	return AIOwner;
}

EBehaviorType& UOBAIComponent::GetCurrentBehaviorType()
{
	return CurrentBehaviorType;
}

TArray<AActor*> UOBAIComponent::GetAIStorageActors()
{
	AGameStateBase* GameState = GetOwner<AGameStateBase>();
	if (GameState)
	{
		UOBAIStorageComponent* AIStorage = GameState->FindComponentByClass<UOBAIStorageComponent>();
		return AIStorage->AIOwners;
	}
	return TArray<AActor*>();
}

//
// SETTERS
//
void UOBAIComponent::SetTargetActor(AActor* NewTarget)
{
	if (AIController)
	{
		if (GetTargetActor() != NewTarget)
			AIController->GetBlackboardComponent()->SetValueAsObject("TargetActor", NewTarget);
	}
}

void UOBAIComponent::SetTargetLocation(FVector InTargetLocation)
{
	if (AIController)
	{
		if (GetTargetLocation() != InTargetLocation)
			AIController->GetBlackboardComponent()->SetValueAsVector("TargetLocation", InTargetLocation);
	}
}

void UOBAIComponent::SetTargetLastLocation(FVector InTargetLocation)
{
	if (AIController)
	{
		if (GetTargetLastLocation() != InTargetLocation)
			AIController->GetBlackboardComponent()->SetValueAsVector("TargetLastLocation", InTargetLocation);
	}
}

/*
void UOBAIComponent::SetControlRotation(AActor* InLookToActor)
{
	if (AIController)
		AIController->SetControlRotation(UKismetMathLibrary::FindLookAtRotation(AIOwner->GetActorLocation(), InLookToActor->GetActorLocation()));
}
*/

//
// BASIC
//
bool UOBAIComponent::CheckIfAliveIfNPC(AActor* ActorRef)
{
	if (ActorRef != nullptr)
	{
		UOBAIComponent* AIComponent = ActorRef->FindComponentByClass<UOBAIComponent>();
		if (AIComponent)
			return AIComponent->IsAlive;
		else
			return true;
	}
	else
		return false;
}

void UOBAIComponent::ChangeBehavior(EBehaviorType InBehaviorType)
{
	if (AIController->GetBrainComponent()->IsRunning() && IsAlive)
	{
		PrevBehaviorType = GetCurrentBehaviorType();

		switch (PrevBehaviorType)
		{
			case EBehaviorType::Idle:
				OnEndIdle.Broadcast();
				break;
			case EBehaviorType::Follow:
				OnEndFollow.Broadcast();
				break;
			case EBehaviorType::Patrol:
				OnEndPatrol.Broadcast();
				break;
			case EBehaviorType::AttackMelee:
				OnEndAttackMelee.Broadcast();
				break;
			case EBehaviorType::Investigate:
				OnEndInvestigate.Broadcast();
				break;
		}

		CurrentBehaviorConfig = BehaviorArray[(uint8)InBehaviorType];
		CurrentBehaviorType = InBehaviorType;

		AIOwner->GetCharacterMovement()->MaxWalkSpeed = CurrentBehaviorConfig.MovementSpeed;

		AIController->GetBlackboardComponent()->SetValueAsEnum("BehaviorType", (uint8)GetCurrentBehaviorType());

		switch (GetCurrentBehaviorType())
		{
			case EBehaviorType::Idle:
				OnBeginIdle.Broadcast();
				break;
			case EBehaviorType::Follow:
				OnBeginFollow.Broadcast();
				break;
			case EBehaviorType::Patrol:
				OnBeginPatrol.Broadcast();
				break;
			case EBehaviorType::AttackMelee:
				OnBeginAttackMelee.Broadcast();
				break;
			case EBehaviorType::Investigate:
				OnBeginInvestigate.Broadcast();
				break;
		}

		return;
	}
	else
	{
		return;
	}
}

void UOBAIComponent::CheckForWithinDistanceTriggers()
{
	for (FActorDistanceTrigger DistanceTrigger : CurrentBehaviorConfig.WithinDistanceTriggers)
	{
		FName ActorTag = DistanceTrigger.ActorTag;
		EBehaviorType BehaviorTo = DistanceTrigger.BehaviorTo;
		float Distance = DistanceTrigger.Distance;
		bool LineOfSight = DistanceTrigger.LineOfSight;

		for (AActor* Actor : GetAIStorageActors())
		{
			if (Actor != nullptr)
			{
				if (LineOfSight)
				{
					TArray<AActor*> PerceivedActors;
					AIPerception->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActors);
					if (PerceivedActors.Contains(Actor))
					{
						if (Actor->ActorHasTag(ActorTag) && Actor->GetDistanceTo(AIOwner) <= Distance)
						{
							SenseLocation = Actor->GetActorLocation();
							ChangeBehavior(BehaviorTo);
						}
					}
				}
				else
				{
					if (Actor->ActorHasTag(ActorTag) && Actor->GetDistanceTo(AIOwner) <= Distance)
					{
						SenseLocation = Actor->GetActorLocation();
						ChangeBehavior(BehaviorTo);
					}
				}
			}
		}
	}
}

void UOBAIComponent::CheckForBeyondDistanceTriggers()
{
	for (FActorDistanceTrigger DistanceTrigger : CurrentBehaviorConfig.BeyondDistanceTriggers)
	{
		FName ActorTag = DistanceTrigger.ActorTag;
		EBehaviorType BehaviorTo = DistanceTrigger.BehaviorTo;
		float Distance = DistanceTrigger.Distance;
		bool LineOfSight = DistanceTrigger.LineOfSight;

		for (AActor* Actor : GetAIStorageActors())
		{
			if (Actor != nullptr)
			{
				if (LineOfSight)
				{
					if (AIController->LineOfSightTo(Actor, FVector(0.f, 0.f, 0.f)))
					{
						if (Actor->ActorHasTag(ActorTag) && Actor->GetDistanceTo(AIOwner) > Distance)
						{
							SenseLocation = Actor->GetActorLocation();
							ChangeBehavior(BehaviorTo);
						}
					}
				}
				else
				{
					if (Actor->ActorHasTag(ActorTag) && Actor->GetDistanceTo(AIOwner) > Distance)
					{
						SenseLocation = Actor->GetActorLocation();
						ChangeBehavior(BehaviorTo);
					}
				}
			}
		}
	}
}

//
// FINDERS
//
bool UOBAIComponent::TryGetNextWaypoint(FVector& outLocation)
{
	if (GetAIPatrolPath())
	{
		const USplineComponent* path = GetAIPatrolPath()->GetSplineComponent();
		if (!path)
			return false;

		const FTransform waypoint = path->GetTransformAtSplinePoint(PatrolSplineIndex, ESplineCoordinateSpace::World);
		if (PatrolSplineIndex > path->GetNumberOfSplinePoints() - 1)
			PatrolSplineIndex = 0;
		else
			PatrolSplineIndex++;

		FVector outLoc;
		if (UNavigationSystemV1::K2_ProjectPointToNavigation(this, waypoint.GetLocation(), outLoc, nullptr, UNavigationQueryFilter::StaticClass()))
			outLocation = outLoc;
		else if (waypoint.GetLocation() != FVector::ZeroVector)
			outLocation = waypoint.GetLocation();

		return true;
	}
	return false;
}

void UOBAIComponent::UpdatePatrolLocation(FVector& outLocation)
{
	if (GetAIPatrolPath() == nullptr)
		return;

	const USplineComponent* path = GetAIPatrolPath()->GetSplineComponent();
	if (path && AIOwner->GetDistanceTo(AIContextActor) < 200.f)
	{
		K2_TrySearchingPatrolPoint();

		if (PatrolSplineIndex > path->GetNumberOfSplinePoints() - 1)
			PatrolSplineIndex = 0;
		else
			PatrolSplineIndex++;

		FVector InTargetLocation = path->GetLocationAtSplinePoint(PatrolSplineIndex, ESplineCoordinateSpace::World);
		GetEQSContextActor()->SetActorLocation(InTargetLocation);

		//AIController->update = true;
		//SetControlRotation(GetEQSContextActor());

		outLocation = InTargetLocation;
	}
}

void UOBAIComponent::UpdatePatrolLocationOnSpline(FVector& outLocation)
{
	if (GetAIPatrolPath() == nullptr)
		return;

	const USplineComponent* path = GetAIPatrolPath()->GetSplineComponent();
	if (path && AIOwner->GetDistanceTo(AIContextActor) < 200.f)
	{
		K2_TrySearchingPatrolPoint();

		PatrolSplineIndex = PatrolSplineIndex + 50;

		float OutDistance = PatrolSplineIndex % FMath::TruncToInt(path->GetSplineLength());
		FVector InTargetLocation = path->GetLocationAtDistanceAlongSpline(OutDistance, ESplineCoordinateSpace::World);
		GetEQSContextActor()->SetActorLocation(InTargetLocation);

		//AIController->bUpdatePawn = true;
		//SetControlRotation(GetEQSContextActor());

		outLocation = InTargetLocation;
	}
}

//
// DEBUG
//
void UOBAIComponent::DebugRenderContextActor()
{
	if (AIContextActor != nullptr)
	{
		::DrawDebugPoint(GetWorld(), AIContextActor->GetActorLocation(), 10.f, FColor::Purple);
		::DrawDebugLine(GetWorld(), AIOwner->GetMesh()->GetSocketLocation("head"), AIContextActor->GetActorLocation(), FColor::Purple);
	}
}

//
// Investigate
// 
//UNavigationSystemV1::K2_GetRandomReachablePointInRadius(AIController, AIController->GetHomeLocation(), outLocation, RandomPatrolRadius)


/*
void UOBAIComponent::FindNextPatrolLocation()
{
	if (PatrolType == EPatrolType::Single)
	{
		PatrolSplineIndex = PatrolSplineIndex + 1;
	}
	else if (PatrolType == EPatrolType::Looping)
	{
		PatrolSplineIndex = PatrolSplineIndex + 1;
		PatrolSplineIndex = PatrolSplineIndex / GetAIPatrolPath()->GetSplinePointNum();
	}
	else
	{
		if (PatrolDirection)
		{
			PatrolSplineIndex = PatrolSplineIndex + 1;

			if (PatrolSplineIndex >= GetAIPatrolPath()->GetSplineComponent()->GetNumberOfSplinePoints())
			{
				PatrolDirection = false;
			}
		}
		else
		{
			PatrolSplineIndex = PatrolSplineIndex - 1;

			if (PatrolSplineIndex < 0)
			{
				PatrolDirection = true;
			}
		}
	}

	FVector InTargetLocation = GetAIPatrolPath()->GetSplineComponent()->GetLocationAtSplinePoint(PatrolSplineIndex, ESplineCoordinateSpace::World);
	SetTargetLocation(InTargetLocation);
}

bool UOBAIComponent::FindPatrolLocationOnSpline()
{
	bool IsAtEnd = false;

	if (PatrolType == EPatrolType::Single)
	{
		PatrolSplineIndex = PatrolSplineIndex + 200;
	}
	else if (PatrolType == EPatrolType::Looping)
	{
		PatrolSplineIndex = PatrolSplineIndex + 200;
		PatrolSplineIndex = PatrolSplineIndex / FMath::TruncToInt(GetAIPatrolPath()->GetSplineComponent()->GetSplineLength());
	}
	else
	{
		if (PatrolDirection)
		{
			PatrolSplineIndex = PatrolSplineIndex + 200;

			IsAtEnd = PatrolSplineIndex >= FMath::TruncToInt(GetAIPatrolPath()->GetSplineComponent()->GetSplineLength());
			if (IsAtEnd)
			{
				PatrolDirection = false;
				PatrolSplineIndex = FMath::TruncToInt(GetAIPatrolPath()->GetSplineComponent()->GetSplineLength());
			}
		}
		else
		{
			PatrolSplineIndex = PatrolSplineIndex - 200;

			IsAtEnd = PatrolSplineIndex < 0;
			if (IsAtEnd)
			{
				PatrolDirection = true;
				PatrolSplineIndex = 0;
			}
		}
	}

	FVector InTargetLocation = GetAIPatrolPath()->GetSplineComponent()->GetLocationAtDistanceAlongSpline((float)PatrolSplineIndex, ESplineCoordinateSpace::World);
	SetTargetLocation(InTargetLocation);

	GetEQSContextActor()->SetActorLocation(InTargetLocation);

	return IsAtEnd;
}
*/

/*
void UOBAIComponent::TickDetection(EPerceptionType InPerceptionType)
{
	if (CurrentBehaviorConfig.WithinDistanceTriggers.Num() > 0 ||
		CurrentBehaviorConfig.BeyondDistanceTriggers.Num() > 0)
	{
		FVector Start = ActorPerceptionInfo.LastSensedStimuli[0].StimulusLocation;
		FVector End = UKismetMathLibrary::Normal(ActorPerceptionInfo.Target->GetVelocity() * 300.f, 0.0001f) + ActorPerceptionInfo.LastSensedStimuli[0].StimulusLocation;

		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(AIOwner);

		bool bHitSucceeded = GetWorld()->LineTraceSingleByChannel(
			Hit,
			Start,
			End,
			ECC_Visibility,
			Params
		);

		if (bHitSucceeded)
			SenseLocation = Hit.Location;
		else
			SenseLocation = Hit.TraceEnd;
	}
}
*/
