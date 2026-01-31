#include "OBAIController.h"
#include "OBAICharacter.h"
#include "OBAIComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBAIController)

AOBAIController::AOBAIController()
{
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	if (PerceptionComponent)
	{
		//
		// SIGHT
		//
		SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

		SightConfig->SightRadius = 1250.f;
		SightConfig->LoseSightRadius = 1750.f;
		SightConfig->PeripheralVisionAngleDegrees = 65.f;
		SightConfig->SetMaxAge(5.f);

		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

		//
		// HEARING
		//
		HearConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearConfig"));

		HearConfig->HearingRange = 1000.f;
		HearConfig->SetMaxAge(5.f);

		HearConfig->DetectionByAffiliation.bDetectEnemies = true;
		HearConfig->DetectionByAffiliation.bDetectNeutrals = false;
		HearConfig->DetectionByAffiliation.bDetectFriendlies = false;

		//
		// CONFIGURE
		//
		PerceptionComponent->ConfigureSense(*SightConfig);
		PerceptionComponent->ConfigureSense(*HearConfig);
		//PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
	}
}

void AOBAIController::Tick(float DeltaTime)
{
	if (!PartialPath)
		PartialPath = HasPartialPath();

	if (AIComponent && AIComponent->GetEQSContextActor() != nullptr)
		AIComponent->DebugRenderContextActor();
}

void AOBAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	CharacterOwned = Cast<AOBAICharacter>(InPawn);
	if (!CharacterOwned)
	{
		UE_LOG(LogTemp, Error, TEXT("OBAIController: Can not Possess. AICharacter not found. Canceled"));
		return;
	}

	if (!PerceptionComponent || !SightConfig || !HearConfig)
	{
		UE_LOG(LogTemp, Error, TEXT("OBAIController: PerceptionComponent or Senses not found. Canceled"));
		return;
	}

	AIComponent = UOBAIComponent::FindAIComponent(CharacterOwned);
	if (!AIComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("OBAIController: AIComponent not found. Canceled"));
		return;
	}
	else
	{
		//PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AOBAIController::OnTargetPerceived);
		//PerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(this, &AOBAIController::OnTargetForgotten);
	}
}

void AOBAIController::OnUnPossess()
{
	Super::OnUnPossess();

	if (!AIComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("OBAIController: AIComponent not found. Canceled"));
		return;
	}
	else
	{
		//PerceptionComponent->OnTargetPerceptionUpdated.RemoveDynamic(this, &AOBAIController::OnTargetPerceived);
		//PerceptionComponent->OnTargetPerceptionForgotten.RemoveDynamic(this, &AOBAIController::OnTargetForgotten);
	}
}

void AOBAIController::UpdateControlRotation(float DeltaTime, bool bUpdatePawn)
{
	Super::UpdateControlRotation(DeltaTime, false);

	if (bUpdatePawn)
	{
		if (APawn* const MyPawn = GetPawn())
		{
			const FRotator CurrentPawnRotation = MyPawn->GetActorRotation();
			SmoothTargetRotation = UKismetMathLibrary::RInterpTo_Constant(MyPawn->GetActorRotation(), ControlRotation, DeltaTime, InterpToControlRotSpeed);
			if (CurrentPawnRotation.Equals(SmoothTargetRotation, 1e-3f) == false)
				MyPawn->FaceRotation(SmoothTargetRotation, DeltaTime);
		}
	}
}

void AOBAIController::OnTargetPerceived(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		if (AIComponent->GetTargetActor() == nullptr)
		{
			for (FActorTrigger ActorTrigger : AIComponent->GetCurrentBehaviorConfig().OnSightTriggers)
			{
				if (Actor->ActorHasTag(ActorTrigger.ActorTag))
				{
					AIComponent->SetTargetActor(Actor);
					GetBlackboardComponent()->ClearValue("TargetLastLocation");

					AIComponent->K2_OnTargetPerceived();

					if (DetectionDelay > 0.f)
						GetWorldTimerManager().SetTimer(DetectionTimerHandle, this, &AOBAIController::TickPerceived, DetectionDelay, false);
					else
						AIComponent->ChangeBehavior(ActorTrigger.BehaviorTo);
				}
			}
		}
	}
	else
	{
		if (AIComponent->GetTargetActor() != nullptr)
		{
			for (FActorTrigger ActorTrigger : AIComponent->GetCurrentBehaviorConfig().LoseSightTriggers)
			{
				if (Actor->ActorHasTag(ActorTrigger.ActorTag))
				{
					AIComponent->SetTargetLastLocation(AIComponent->GetTargetActor()->GetActorLocation());
					GetBlackboardComponent()->ClearValue("TargetActor");

					AIComponent->K2_OnTargetLost();

					if (DetectionDelay > 0.f)
						GetWorldTimerManager().SetTimer(DetectionTimerHandle, this, &AOBAIController::TickLost, DetectionDelay * 2, false);
					else
						AIComponent->ChangeBehavior(ActorTrigger.BehaviorTo);
				}
			}
		}
	}
}

void AOBAIController::TickPerceived()
{

}

void AOBAIController::TickLost()
{

}

void AOBAIController::OnTargetForgotten(AActor* Actor)
{
	GetBlackboardComponent()->ClearValue("TargetActor");
	GetBlackboardComponent()->ClearValue("TargetLastLocation");

	AIComponent->K2_OnTargetForgotten();

	/*
	if (PerceptionReaction.OnForgottenSound != nullptr)
		UGameplayStatics::PlaySoundAtLocation(this, PerceptionReaction.OnForgottenSound, GetOwner()->GetActorLocation());
	if (PerceptionReaction.OnForgottenMontage != nullptr)
		Cast<ACharacter>(GetPawn())->GetMesh()->GetAnimInstance()->Montage_Play(PerceptionReaction.OnForgottenMontage, 1.0f);
	*/
}

/*
void AOBAIController::TickPerceived()
{
	if (CurrentDetectionLevel == 1.f)
	{
		GetWorld()->GetTimerManager().ClearTimer(DetectionTimerHandle);
		GetBlackboardComponent()->SetValueAsObject("TargetActor", PerceivedTarget);
		AIComponent->K2_OnTargetPerceived();

		if (PerceptionReaction.OnPerceivedSound != nullptr)
			UGameplayStatics::PlaySoundAtLocation(this, PerceptionReaction.OnPerceivedSound, GetOwner()->GetActorLocation());
		if (PerceptionReaction.OnPerceivedMontage != nullptr)
			Cast<ACharacter>(GetPawn())->GetMesh()->GetAnimInstance()->Montage_Play(PerceptionReaction.OnPerceivedMontage, 1.0f);

		for (FActorTrigger ActorTrigger : AIComponent->GetCurrentBehaviorConfig().OnSightTriggers)
		{
			AIComponent->ChangeBehavior(ActorTrigger.BehaviorTo);
		}

		RootBehavior();
	}

	CurrentDetectionLevel = FMath::Clamp(CurrentDetectionLevel + (GetWorld()->GetDeltaSeconds() / DetectionSpeedDelay), 0.0f, 1.0f);
	OnTargetPerceivedDelegate.Broadcast(CurrentDetectionLevel);
}

void AOBAIController::TickLost()
{
	if (CurrentDetectionLevel == 0.f)
	{
		GetWorld()->GetTimerManager().ClearTimer(DetectionTimerHandle);
		GetBlackboardComponent()->ClearValue("TargetActor");
		AIComponent->K2_OnTargetLost();

		if (PerceptionReaction.OnLostSound != nullptr)
			UGameplayStatics::PlaySoundAtLocation(this, PerceptionReaction.OnLostSound, GetOwner()->GetActorLocation());
		if (PerceptionReaction.OnLostMontage != nullptr)
			Cast<ACharacter>(GetPawn())->GetMesh()->GetAnimInstance()->Montage_Play(PerceptionReaction.OnLostMontage, 1.0f);

		for (FActorTrigger ActorTrigger : AIComponent->GetCurrentBehaviorConfig().LoseSightTriggers)
		{
			AIComponent->ChangeBehavior(ActorTrigger.BehaviorTo);
		}

		RootBehavior();
	}

	CurrentDetectionLevel = FMath::Clamp(CurrentDetectionLevel + -(GetWorld()->GetDeltaSeconds() / DetectionDecayDelay), 0.0f, 1.0f);
	OnTargetLostDelegate.Broadcast(CurrentDetectionLevel);
}

//
// WIP
//
void AOBAIController::RootBehavior()
{
	UWorld* World = GetWorld();

	UCharacterMovementComponent* MoveComp = Cast<UCharacterMovementComponent>(Cast<ACharacter>(GetPawn())->GetCharacterMovement());

	if (!ensure(MoveComp))
		return;

	switch (AIComponent->GetCurrentBehaviorType())
	{
		case EBehaviorType::Follow:

			MoveComp->RotationRate = FRotator(0.f, CurrentDetectionLevel == 0.f ? 48.f : 96.f, 0.f);

			break;
	}
}
*/