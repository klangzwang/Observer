#pragma once
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "OBCharacterMovementComponent.generated.h"

UCLASS(Config = Game)
class OBSERVERFRAMEWORK_API UOBCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:

	UOBCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/*
public:

	virtual bool CanAttemptJump() const override;

	virtual FRotator GetDeltaRotation(float DeltaTime) const override;
	virtual float GetMaxSpeed() const override;

public:

	UFUNCTION(BlueprintCallable, Category = "Observer|Input", meta = (Keywords = "GetMovementInput GetInput"))
	FVector GetLastMovementInputVector() const;

protected:

	UPROPERTY(Transient)
	FVector ControlInputVector;

	UPROPERTY(Transient)
	FVector LastControlInputVector;
	*/

//
// BASIC
//
	/*
	bool UUTCharacterMovement::CanDodge()
	{
		if (GetCurrentMovementTime() < DodgeResetTime)
		{
			UE_LOG(UTNet, Warning, TEXT("Failed dodge current move time %f dodge reset time %f"), GetCurrentMovementTime(), DodgeResetTime);
		}
		else
		{
			UE_LOG(UTNet, Warning, TEXT("SUCCEEDED candodge current move time %f dodge reset time %f"), GetCurrentMovementTime(), DodgeResetTime);
		}
		
		return !bIsFloorSliding && CanEverJump() && (GetCurrentMovementTime() > DodgeResetTime);
	}

	bool UUTCharacterMovement::CanWallDodge(const FVector& DodgeDir, const FVector& DodgeCross, FHitResult& Result, bool bIsLowGrav)
	{
		FVector TraceEnd = -1.f * DodgeDir;
		float PawnCapsuleRadius, PawnCapsuleHalfHeight;
		CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleSize(PawnCapsuleRadius, PawnCapsuleHalfHeight);
		float TraceBoxSize = FMath::Min(0.25f * PawnCapsuleHalfHeight, 0.7f * PawnCapsuleRadius);
		FVector TraceStart = CharacterOwner->GetActorLocation();
		TraceStart.Z -= 0.5f * TraceBoxSize;
		TraceEnd = TraceStart - (WallDodgeTraceDist + PawnCapsuleRadius - 0.5f * TraceBoxSize) * DodgeDir;

		static const FName DodgeTag = FName(TEXT("Dodge"));
		FCollisionQueryParams QueryParams(DodgeTag, false, CharacterOwner);
		const bool bBlockingHit = GetWorld()->SweepSingleByChannel(Result, TraceStart, TraceEnd, FQuat::Identity, UpdatedComponent->GetCollisionObjectType(), FCollisionShape::MakeSphere(TraceBoxSize), QueryParams);
		if (!bBlockingHit || Cast<ACharacter>(Result.Actor.Get()) || (!IsSwimming() && (CurrentWallDodgeCount > 0) && !bIsLowGrav && ((Result.ImpactNormal | LastWallDodgeNormal) > MaxConsecutiveWallDodgeDP)))
		{
			//UE_LOG(UTNet, Warning, TEXT("No wall to dodge"));
			return false;
		}
		return true;
	}

	bool UUTCharacterMovement::PerformDodge(FVector& DodgeDir, FVector& DodgeCross)
	{
		if (!HasValidData())
		{
			return false;
		}

		FVector Loc = CharacterOwner->GetActorLocation();
		UE_LOG(UTNet, Warning, TEXT("Perform dodge at %f loc %f %f %f vel %f %f %f dodgedir %f %f %f from yaw %f"), GetCurrentSynchTime(), Loc.X, Loc.Y, Loc.Z, Velocity.X, Velocity.Y, Velocity.Z, DodgeDir.X, DodgeDir.Y, DodgeDir.Z, CharacterOwner->GetActorRotation().Yaw);

		float HorizontalImpulse = DodgeImpulseHorizontal;
		bool bIsLowGrav = (GetGravityZ() > UPhysicsSettings::Get()->DefaultGravityZ);
		AUTCharacter* UTCharOwner = Cast<AUTCharacter>(CharacterOwner);
		if (UTCharOwner)
		{
			bIsLowGrav = !UTCharOwner->bApplyWallSlide && bIsLowGrav;
		}
		NeedsClientAdjustment();
		bool bIsAWallDodge = false;
		bool bIsALiftJump = false;
		if (!IsMovingOnGround())
		{
			if (IsFalling() && (CurrentWallDodgeCount >= MaxWallDodges))
			{
				//UE_LOG(UTNet, Warning, TEXT("Exceeded max wall dodges"));
				return false;
			}
			// if falling/swimming, check if can perform wall dodge
			FHitResult Result;
			if (!CanWallDodge(DodgeDir, DodgeCross, Result, bIsLowGrav))
			{
				return false;
			}
			if ((Result.ImpactNormal | DodgeDir) < WallDodgeMinNormal)
			{
				// clamp dodge direction based on wall normal
				FVector ForwardDir = (Result.ImpactNormal ^ FVector(0.f, 0.f, 1.f)).GetSafeNormal();
				if ((ForwardDir | DodgeDir) < 0.f)
				{
					ForwardDir *= -1.f;
				}
				DodgeDir = Result.ImpactNormal * WallDodgeMinNormal * WallDodgeMinNormal + ForwardDir * (1.f - WallDodgeMinNormal * WallDodgeMinNormal);
				DodgeDir = DodgeDir.GetSafeNormal();
				FVector NewDodgeCross = (DodgeDir ^ FVector(0.f, 0.f, 1.f)).GetSafeNormal();
				DodgeCross = ((NewDodgeCross | DodgeCross) < 0.f) ? -1.f * NewDodgeCross : NewDodgeCross;
			}
			DodgeResetTime = GetCurrentMovementTime() + WallDodgeResetInterval;
			HorizontalImpulse = IsSwimming() ? SwimmingWallPushImpulse : WallDodgeImpulseHorizontal;
			CurrentWallDodgeCount++;
			LastWallDodgeNormal = Result.ImpactNormal;
			bIsAWallDodge = true;
			bCountWallSlides = true;
			bHasPlayedWallHitSound = false;
		}
		else if (!GetImpartedMovementBaseVelocity().IsZero())
		{
			// lift jump counts as wall dodge
			CurrentWallDodgeCount++;
			LastWallDodgeNormal = FVector(0.f, 0.f, 1.f);
			DodgeResetTime = GetCurrentMovementTime() + WallDodgeResetInterval;
			bIsALiftJump = true;
		}
		float VelocityZ = Velocity.Z;
		float MaxHorizontalVelocity = DodgeMaxHorizontalVelocity;
		// perform the dodge
		AUTPlayerState* UTPlayerState = UTCharOwner ? Cast<AUTPlayerState>(UTCharOwner->PlayerState) : nullptr;
		bool bSlowMovement = (UTPlayerState && UTPlayerState->CarriedObject && UTPlayerState->CarriedObject->bSlowsMovement);
		if (bSlowMovement)
		{
			HorizontalImpulse *= 0.8f;
			MaxHorizontalVelocity *= 0.8f;
		}
		Velocity = HorizontalImpulse * DodgeDir + (Velocity | DodgeCross) * DodgeCross;
		Velocity.Z = 0.f;
		float SpeedXY = FMath::Min(Velocity.Size(), MaxHorizontalVelocity);
		SpeedXY *= UTCharOwner ? UTCharOwner->MaxSpeedPctModifier : 1.0f;
		Velocity = SpeedXY * Velocity.GetSafeNormal();

		if (bSlowMovement)
		{
			Velocity.Z = IsMovingOnGround() ? 400.f : 200.f;
		}
		else if (IsMovingOnGround())
		{
			Velocity.Z = DodgeImpulseVertical;
		}
		else if (!IsSwimming() && (VelocityZ < MaxAdditiveDodgeJumpSpeed) && (VelocityZ > MinAdditiveDodgeFallSpeed))
		{
			float CurrentWallImpulse = (CurrentWallDodgeCount < 2) ? WallDodgeImpulseVertical : WallDodgeSecondImpulseVertical;

			if (!bIsLowGrav && (CurrentWallDodgeCount > 1))
			{
				VelocityZ = FMath::Min(0.f, VelocityZ);
			}
			else if ((VelocityZ < 0.f) && (VelocityZ > WallDodgeGraceVelocityZ))
			{
				if (Velocity.Z < -1.f * JumpZVelocity)
				{
					CurrentWallImpulse = FMath::Max(WallDodgeSecondImpulseVertical, WallDodgeImpulseVertical + Velocity.Z + JumpZVelocity);
				}
				// allowing dodge with loss of downward velocity is no free lunch for falling damage
				FHitResult Hit(1.f);
				Hit.ImpactNormal = FVector(0.f, 0.f, 1.f);
				Hit.Normal = Hit.ImpactNormal;
				Cast<AUTCharacter>(CharacterOwner)->TakeFallingDamage(Hit, VelocityZ - CurrentWallImpulse);
				VelocityZ = 0.f;
			}
			Velocity.Z = FMath::Min(VelocityZ + CurrentWallImpulse, MaxAdditiveDodgeJumpSpeed);
			UE_LOG(UTNet, Warning, TEXT("Wall dodge at %f velZ %f"), CharacterOwner->GetWorld()->GetTimeSeconds(), Velocity.Z);
		}
		else
		{
			Velocity.Z = VelocityZ;
		}
		bExplicitJump = true;
		bIsDodging = true;
		bNotifyApex = true;
		if (IsMovingOnGround())
		{
			SetMovementMode(MOVE_Falling);
		}
		AUTPlayerState* PS = CharacterOwner ? Cast<AUTPlayerState>(CharacterOwner->PlayerState) : NULL;
		if (PS)
		{
			PS->ModifyStatsValue(bIsAWallDodge ? NAME_NumWallDodges : NAME_NumDodges, 1);
			if (bIsALiftJump)
			{
				PS->ModifyStatsValue(NAME_NumLiftJumps, 1);
			}
		}
		return true;
	}

	void UUTCharacterMovement::HandlePressedSlide()
	{
		bPressedSlide = true;
		if (IsMovingOnGround())
		{
			if (GetCurrentMovementTime() > FloorSlideEndTime + DodgeResetInterval)
			{
				CharacterOwner->bPressedJump = true;
			}
			else
			{
				bPressedSlide = false;
			}
		}
	}

	void UUTCharacterMovement::HandleSlideRequest()
	{
		AUTCharacter* UTCharacterOwner = Cast<AUTCharacter>(CharacterOwner);
		if (!Acceleration.IsNearlyZero() && (Velocity.Size() > 0.5f * MaxWalkSpeed) && UTCharacterOwner && UTCharacterOwner->CanSlide())
		{
			HandlePressedSlide();
		}
	}

	void UUTCharacterMovement::HandleCrouchRequest()
	{
		// if moving fast enough and pressing on move key, slide, else crouch
		AUTCharacter* UTCharacterOwner = Cast<AUTCharacter>(CharacterOwner);
		if (!Acceleration.IsNearlyZero() && (Velocity.Size() > 0.5f * MaxWalkSpeed) && UTCharacterOwner && UTCharacterOwner->CanSlide())
		{
			AUTPlayerController* PC = Cast<AUTPlayerController>(UTCharacterOwner->GetController());
			if (PC && PC->bCrouchTriggersSlide)
			{
				HandlePressedSlide();
			}
		}
		bWantsToCrouch = true;
	}

	void UUTCharacterMovement::HandleUnCrouchRequest()
	{
		bWantsToCrouch = false;
		AUTPlayerController* PC = CharacterOwner ? Cast<AUTPlayerController>(CharacterOwner->GetController()) : nullptr;
		if (!PC || PC->bCrouchTriggersSlide)
		{
			UpdateFloorSlide(false);
		}
	}

	void UUTCharacterMovement::Crouch(bool bClientSimulation)
	{
		Super::Crouch(bClientSimulation);
		if (!bIsFloorSliding && CharacterOwner && CharacterOwner->bIsCrouched && (Velocity.Size2D() > MaxWalkSpeedCrouched))
		{
			float SavedVelZ = Velocity.Z;
			Velocity = MaxWalkSpeedCrouched * Velocity.GetSafeNormal2D();
			Velocity.Z = SavedVelZ;
		}
	}

	void UUTCharacterMovement::PerformFloorSlide(const FVector& DodgeDir, const FVector& FloorNormal)
	{
		if (CharacterOwner)
		{
			FloorSlideTapTime = GetCurrentMovementTime();
			bIsFloorSliding = true;
			FloorSlideEndTime = GetCurrentMovementTime() + FloorSlideDuration;
			Acceleration = FloorSlideAcceleration * DodgeDir;
			DodgeResetTime = FloorSlideEndTime + DodgeResetInterval;
			float NewSpeed = FMath::Max(MaxFloorSlideSpeed, FMath::Min(Velocity.Size2D(), MaxInitialFloorSlideSpeed));
			if ((NewSpeed > MaxFloorSlideSpeed) && ((DodgeDir | FloorNormal) < 0.f))
			{
				// don't allow sliding up steep slopes at faster than MaxFloorSlideSpeed
				NewSpeed = FMath::Clamp(NewSpeed + FloorSlideSlopeBraking * (NewSpeed - MaxFloorSlideSpeed) * (DodgeDir | FloorNormal), MaxFloorSlideSpeed, NewSpeed);
			}
			Velocity = NewSpeed * DodgeDir;
			AUTCharacter* UTChar = Cast<AUTCharacter>(CharacterOwner);
			if (UTChar)
			{
				UTChar->MovementEventUpdated(EME_Slide, DodgeDir);
				if (UTChar->Role != ROLE_Authority)
				{
					UTChar->bRepFloorSliding = true;
				}
			}
			AUTPlayerState* PS = CharacterOwner ? Cast<AUTPlayerState>(CharacterOwner->PlayerState) : NULL;
			if (PS)
			{
				PS->ModifyStatsValue(NAME_NumFloorSlides, 1);
			}
		}
	}

	bool UUTCharacterMovement::IsCrouching() const
	{
		return CharacterOwner && CharacterOwner->bIsCrouched && !bIsFloorSliding;
	}

	void UUTCharacterMovement::PerformMovement(float DeltaSeconds)
	{
		if (!CharacterOwner)
		{
			return;
		}
		AUTCharacter* UTOwner = Cast<AUTCharacter>(CharacterOwner);
		bSlidingAlongWall = false;
		if (!UTOwner || !UTOwner->IsRagdoll())
		{
			FVector OldVelocity = Velocity;
			float RealGroundFriction = GroundFriction;
			if (Acceleration.IsZero())
			{
				GroundFriction = BrakingFriction;
			}
			if (bIsFloorSliding)
			{
				GroundFriction = 0.f;
				BrakingDecelerationWalking = BrakingDecelerationSliding;
			}
			else if (bWasFloorSliding && (MovementMode != MOVE_Falling))
			{
				Velocity *= FloorSlideEndingSpeedFactor;
			}
			bWasFloorSliding = bIsFloorSliding;

			bool bSavedWantsToCrouch = bWantsToCrouch;
			bWantsToCrouch = bWantsToCrouch || bIsFloorSliding;
			bForceMaxAccel = bIsFloorSliding;

			FVector Loc = CharacterOwner->GetActorLocation();
			if (CharacterOwner->Role < ROLE_Authority)
			{
			UE_LOG(UTNet, Warning, TEXT("CLIENT MOVE at %f deltatime %f from %f %f %f vel %f %f %f accel %f %f %f wants to crouch %d sliding %d sprinting %d pressed slide %d"), GetCurrentSynchTime(), DeltaSeconds, Loc.X, Loc.Y, Loc.Z, Velocity.X, Velocity.Y, Velocity.Z, Acceleration.X, Acceleration.Y, Acceleration.Z, bWantsToCrouch, bIsFloorSliding, bIsSprinting, bPressedSlide);
			}
			else
			{
			UE_LOG(UTNet, Warning, TEXT("SERVER Move at %f deltatime %f from %f %f %f vel %f %f %f accel %f %f %f wants to crouch %d sliding %d sprinting %d pressed slide %d"), GetCurrentSynchTime(), DeltaSeconds, Loc.X, Loc.Y, Loc.Z, Velocity.X, Velocity.Y, Velocity.Z, Acceleration.X, Acceleration.Y, Acceleration.Z, bWantsToCrouch, bIsFloorSliding, bIsSprinting, bPressedSlide);
			}

			FVector StartMoveLoc = GetActorLocation();
			Super::PerformMovement(DeltaSeconds);
			UpdateMovementStats(StartMoveLoc);
			bWantsToCrouch = bSavedWantsToCrouch;
			GroundFriction = RealGroundFriction;
			BrakingDecelerationWalking = DefaultBrakingDecelerationWalking;

			if (Velocity.Size() > 0.f)
			{
							UE_LOG(UT, Warning, TEXT("Delta %f Velocity %s Speed %f AccelRate %f"), DeltaSeconds, *Velocity.ToString(), Velocity.Size(), (Velocity - OldVelocity).Size() / DeltaSeconds);
			}
		}

		if (UTOwner != NULL)
		{
			UTOwner->PositionUpdated(bShotSpawned);
			bShotSpawned = false;
			// tick movement reduction timer
			UTOwner->WalkMovementReductionTime = FMath::Max(0.f, UTOwner->WalkMovementReductionTime - DeltaSeconds);
			if (UTOwner->WalkMovementReductionTime <= 0.0f)
			{
				UTOwner->WalkMovementReductionPct = 0.0f;
			}
		}
	}

	void UUTCharacterMovement::UpdateMovementStats(const FVector& StartLocation)
	{
		if (CharacterOwner && CharacterOwner->Role == ROLE_Authority)
		{
			AUTPlayerState* PS = Cast<AUTPlayerState>(CharacterOwner->PlayerState);
			if (PS)
			{
				float Dist = (GetActorLocation() - StartLocation).Size();
				FName MovementName = bIsSprinting ? NAME_SprintDist : NAME_RunDist;
				if (MovementMode == MOVE_Falling)
				{
					AUTCharacter* UTCharOwner = Cast<AUTCharacter>(CharacterOwner);
					MovementName = (UTCharOwner && UTCharOwner->bApplyWallSlide) ? NAME_WallRunDist : NAME_InAirDist;
				}
				else if (MovementMode == MOVE_Swimming)
				{
					MovementName = NAME_SwimDist;
				}
				else if (bIsFloorSliding)
				{
					MovementName = NAME_SlideDist;
				}
				PS->ModifyStatsValue(MovementName, Dist);
			}
		}
	}

	float UUTCharacterMovement::GetMaxAcceleration() const
	{
		float Result;
		if (bIsFloorSliding)
		{
			Result = FloorSlideAcceleration;
		}
		else if (MovementMode == MOVE_Falling)
		{
			Result = MaxFallingAcceleration;
		}
		else if (MovementMode == MOVE_Swimming)
		{
			Result = MaxSwimmingAcceleration + MaxRelativeSwimmingAccelNumerator / (Velocity.Size() + MaxRelativeSwimmingAccelDenominator);
		}
		else
		{
			Result = Super::GetMaxAcceleration();
			if (bIsSprinting && Velocity.SizeSquared() > FMath::Square<float>(MaxWalkSpeed))
			{
				// smooth transition to sprinting accel to avoid client/server synch issues
				const float CurrentSpeed = Velocity.Size();
				const float Transition = FMath::Min(1.f, 0.1f * (CurrentSpeed - MaxWalkSpeed));
				Result = SprintAccel * Transition + Result * (1.f - Transition);
			}
			else if (Velocity.SizeSquared() < MaxFastAccelSpeed * MaxFastAccelSpeed)
			{
				//extra accel to start, smooth to avoid synch issues
				const float CurrentSpeed = Velocity.Size();
				const float Transition = FMath::Min(1.f, CurrentSpeed / MaxFastAccelSpeed);
				Result = Result * Transition + FastInitialAcceleration * (1.f - Transition);
			}
		}
		if (MovementMode == MOVE_Walking && Cast<AUTCharacter>(CharacterOwner) != NULL)
		{
			Result *= (1.0f - ((AUTCharacter*)CharacterOwner)->GetWalkMovementReductionPct());
		}
		return Result;
	}

	bool UUTCharacterMovement::CanSprint() const
	{
		if (CharacterOwner && IsMovingOnGround() && !IsCrouching() && (GetCurrentMovementTime() > SprintStartTime))
		{
			AUTPlayerState* UTPlayerState = CharacterOwner ? Cast<AUTPlayerState>(CharacterOwner->PlayerState) : nullptr;
			if (UTPlayerState && UTPlayerState->CarriedObject && UTPlayerState->CarriedObject->bSlowsMovement)
			{
				return false;
			}
			// must be moving mostly forward
			FRotator TurnRot(0.f, CharacterOwner->GetActorRotation().Yaw, 0.f);
			FVector X = FRotationMatrix(TurnRot).GetScaledAxis(EAxis::X);
			return (((X | Velocity.GetSafeNormal()) > 0.8f) && ((X | Acceleration.GetSafeNormal()) > 0.9f));
		}
		return false;
	}

	float UUTCharacterMovement::GetMaxSpeed() const
	{
		float FinalMaxSpeed = 0.0f;

		// ignore standard movement while character is a ragdoll
		if (Cast<AUTCharacter>(CharacterOwner) != NULL && ((AUTCharacter*)CharacterOwner)->IsRagdoll())
		{
			// small non-zero number used to avoid divide by zero issues
			FinalMaxSpeed = 0.01f;
		}
		else if (bIsTaunting)
		{
			FinalMaxSpeed = 0.01f;
		}
		else if (bIsFloorSliding && (MovementMode == MOVE_Walking))
		{
			// higher max velocity if going down hill
			float CurrentSpeed = Velocity.Size();
			if ((CurrentSpeed > MaxFloorSlideSpeed) && (CurrentFloor.HitResult.ImpactNormal.Z < 1.f))
			{
				float TopSlideSpeed = FMath::Min(MaxInitialFloorSlideSpeed, CurrentSpeed);
				FinalMaxSpeed = FMath::Min(TopSlideSpeed, MaxFloorSlideSpeed + FMath::Max(0.f, (Velocity | CurrentFloor.HitResult.ImpactNormal)));
			}
			else
			{
				FinalMaxSpeed = MaxFloorSlideSpeed;
			}
		}
		else if (bFallingInWater && (MovementMode == MOVE_Falling))
		{
			FinalMaxSpeed = MaxWaterSpeed;
		}
		else if (MovementMode == MOVE_Swimming)
		{
			AUTWaterVolume* WaterVolume = Cast<AUTWaterVolume>(GetPhysicsVolume());
			FinalMaxSpeed = WaterVolume ? FMath::Min(MaxSwimSpeed, WaterVolume->MaxRelativeSwimSpeed) : MaxSwimSpeed;
		}
		else
		{
			FinalMaxSpeed = bIsSprinting ? SprintSpeed : Super::GetMaxSpeed();
		}

		AUTCharacter* UTCharOwner = Cast<AUTCharacter>(CharacterOwner);
		if (UTCharOwner)
		{
			return FinalMaxSpeed * UTCharOwner->MaxSpeedPctModifier;
		}

		return FinalMaxSpeed;
	}

	void UUTCharacterMovement::ApplyVelocityBraking(float DeltaTime, float Friction, float BrakingDeceleration)
	{
		if (Acceleration.IsZero())
		{
			SprintStartTime = GetCurrentMovementTime() + AutoSprintDelayInterval;
		}
		Super::ApplyVelocityBraking(DeltaTime, Friction, BrakingDeceleration);
	}

	void UUTCharacterMovement::CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration)
	{
		if (MovementMode == MOVE_Walking)
		{
			AUTCharacter* UTOwner = Cast<AUTCharacter>(CharacterOwner);
			if (UTOwner != NULL)
			{
				Friction *= (1.0f - UTOwner->GetWalkMovementReductionPct());
				BrakingDeceleration *= (1.0f - UTOwner->GetWalkMovementReductionPct());
			}
			if (!bIsFloorSliding)
			{
				// always clamp to max speed (gets back down from sprinting faster, so get full accel)
				float MaxSpeed = GetMaxSpeed();
				if (Velocity.SizeSquared() > MaxSpeed * MaxSpeed)
				{
					Velocity = Velocity.GetSafeNormal() * MaxSpeed;
				}
			}
		}
		Super::CalcVelocity(DeltaTime, Friction, bFluid, BrakingDeceleration);
		UE_LOG(UTNet, Warning, TEXT("At %f DeltaTime %f Velocity is %f %f %f from acceleration %f %f slide %d"), GetCurrentSynchTime(), DeltaTime, Velocity.X, Velocity.Y, Velocity.Z, Acceleration.X, Acceleration.Y, bIsFloorSliding);

		// workaround for engine path following code not setting Acceleration correctly
		if (bHasRequestedVelocity && Acceleration.IsZero())
		{
			Acceleration = Velocity.GetSafeNormal();
		}
	}

	void UUTCharacterMovement::ResetTimers()
	{
		DodgeResetTime = 0.f;
		SprintStartTime = GetCurrentMovementTime() + AutoSprintDelayInterval;
		FloorSlideTapTime = 0.f;
		FloorSlideEndTime = 0.f;
		GetWorld()->GetTimerManager().ClearTimer(FloorSlideTapHandle);
	}

	void UUTCharacterMovement::CheckJumpInput(float DeltaTime)
	{
		if (bPressedSlide)
		{
			UpdateFloorSlide(true);
		}
		if (CharacterOwner && CharacterOwner->bPressedJump)
		{
			if ((MovementMode == MOVE_Walking) || (MovementMode == MOVE_Falling))
			{
				DoJump(CharacterOwner->bClientUpdating);
			}
			else if ((MovementMode == MOVE_Swimming) && CanDodge())
			{
				PerformWaterJump();
			}
		}
		else if (bPressedDodgeForward || bPressedDodgeBack || bPressedDodgeLeft || bPressedDodgeRight)
		{
			AUTCharacter* UTCharacterOwner = Cast<AUTCharacter>(CharacterOwner);
			if (UTCharacterOwner)
			{
				FVector DodgeDir, DodgeCross;
				GetDodgeDirection(DodgeDir, DodgeCross);
				UTCharacterOwner->Dodge(DodgeDir, DodgeCross);
			}
		}

		if (CharacterOwner)
		{
			// If server, we already got these flags from the saved move
			if (CharacterOwner->IsLocallyControlled())
			{
				bIsFloorSliding = bIsFloorSliding && (GetCurrentMovementTime() < FloorSlideEndTime);
				//bool bWasSprinting = bIsSprinting;
				bIsSprinting = CanSprint();
				AUTCharacter* UTCharOwner = Cast<AUTCharacter>(CharacterOwner);
				if (UTCharOwner && (UTCharOwner->Role != ROLE_Authority))
				{
					UTCharOwner->bRepFloorSliding = bIsFloorSliding;
				}
			}

			if (!bIsFloorSliding && bWasFloorSliding)
			{
				SprintStartTime = GetCurrentMovementTime() + AutoSprintDelayInterval;
				AUTCharacter* UTCharacterOwner = Cast<AUTCharacter>(CharacterOwner);
				if (UTCharacterOwner)
				{
					UTCharacterOwner->UpdateCrouchedEyeHeight();
					UTCharacterOwner->DesiredJumpBob = FVector(0.f);
				}
			}
		}
	}

	bool UUTCharacterMovement::CanCrouchInCurrentState() const
	{
		return CanEverCrouch() && IsMovingOnGround();
	}

//
// SLIDE
//
	void UUTCharacterMovement::UpdateWallSlide(bool bNewWantsWallSlide)
	{
		bWantsWallSlide = bNewWantsWallSlide;
	}

	void UUTCharacterMovement::UpdateFloorSlide(bool bNewWantsFloorSlide)
	{
		if (bNewWantsFloorSlide && !bWantsFloorSlide)
		{
			bSlideFromGround = IsMovingOnGround();
			FloorSlideTapTime = GetCurrentMovementTime();
			GetWorld()->GetTimerManager().ClearTimer(FloorSlideTapHandle);
		}
		else if (!bNewWantsFloorSlide && bWantsFloorSlide && (GetCurrentMovementTime() - FloorSlideTapTime < 0.25f))
		{
			// delay clearing bWantsFloorSlide after quick taps, to allow slightly early taps for landing slides
			GetWorld()->GetTimerManager().SetTimer(FloorSlideTapHandle, this, &UUTCharacterMovement::ClearFloorSlideTap, (bSlideFromGround ? 0.8f : FloorSlideBonusTapInterval), false);
			return;
		}
		bWantsFloorSlide = bNewWantsFloorSlide;
	}

	void UUTCharacterMovement::ClearFloorSlideTap()
	{
		bWantsFloorSlide = false;
	}

	bool UUTCharacterMovement::WantsFloorSlide()
	{
		return bWantsFloorSlide;
	}

	bool UUTCharacterMovement::WantsWallSlide()
	{
		return bWantsWallSlide;
	}

	FVector UUTCharacterMovement::ComputeSlideVectorUT(const float DeltaTime, const FVector& InDelta, const float Time, const FVector& Normal, const FHitResult& Hit)
	{
		const bool bFalling = IsFalling();
		FVector Delta = InDelta;
		FVector Result = UMovementComponent::ComputeSlideVector(Delta, Time, Normal, Hit);

		// prevent boosting up slopes
		if (bFalling && Result.Z > 0.f)
		{
			// @TODO FIXMESTEVE - make this a virtual function in super class so just change this part
			float PawnRadius, PawnHalfHeight;
			CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleSize(PawnRadius, PawnHalfHeight);
			if (Delta.Z < 0.f && (Hit.ImpactNormal.Z < MAX_STEP_SIDE_Z))
			{
				// We were moving downward, but a slide was going to send us upward. We want to aim
				// straight down for the next move to make sure we get the most upward-facing opposing normal.
				Result = FVector(0.f, 0.f, Delta.Z);
			}
			else if (bAllowSlopeDodgeBoost && (((CharacterOwner->GetActorLocation() - Hit.ImpactPoint).Size2D() > 0.93f * PawnRadius) || (Hit.ImpactNormal.Z > 0.2f))) // @TODO FIXMESTEVE tweak magic numbers
			{
				if (Result.Z > Delta.Z * Time)
				{
					Result.Z = FMath::Max(Result.Z * SlopeDodgeScaling, Delta.Z * Time);
				}
			}
			else
			{
				// Don't move any higher than we originally intended.
				const float ZLimit = Delta.Z * Time;
				if (Result.Z > ZLimit && ZLimit > KINDA_SMALL_NUMBER)
				{
					FVector SlideResult = Result;

					// Rescale the entire vector (not just the Z component) otherwise we change the direction and likely head right back into the impact.
					const float UpPercent = ZLimit / Result.Z;
					Result *= UpPercent;

					// Make remaining portion of original result horizontal and parallel to impact normal.
					const FVector RemainderXY = (SlideResult - Result) * FVector(1.f, 1.f, 0.f);
					const FVector NormalXY = Normal.GetSafeNormal2D();
					const FVector Adjust = Super::ComputeSlideVector(RemainderXY, 1.f, NormalXY, Hit);
					Result += Adjust;
				}
			}
		}
		return Result;
	}

	void UUTCharacterMovement::CheckWallSlide(FHitResult const& Impact)
	{
		AUTCharacter* UTCharOwner = Cast<AUTCharacter>(CharacterOwner);
		if (UTCharOwner)
		{
			UTCharOwner->bApplyWallSlide = false;
			if (bWantsWallSlide && (Impact.ImpactNormal.Z > -0.1f) && (Velocity.Z < MaxSlideRiseZ) && (Velocity.Z > MaxSlideFallZ) && !Acceleration.IsZero() && !UTCharOwner->IsThirdPersonTaunting())
			{
				FVector VelocityAlongWall = Velocity + FMath::Abs(Velocity | Impact.ImpactNormal) * Impact.ImpactNormal;
				UTCharOwner->bApplyWallSlide = (VelocityAlongWall.Size2D() >= MinWallSlideSpeed);
				if (UTCharOwner->bApplyWallSlide && bCountWallSlides)
				{
					bCountWallSlides = false;
					AUTPlayerState* PS = CharacterOwner ? Cast<AUTPlayerState>(CharacterOwner->PlayerState) : NULL;
					if (PS)
					{
						PS->ModifyStatsValue(NAME_NumWallRuns, 1);
					}
				}
			}
		}
	}

	float UUTCharacterMovement::GetGravityZ() const
	{
		AUTCharacter* UTCharOwner = Cast<AUTCharacter>(CharacterOwner);
		if (UTCharOwner && UTCharOwner->bApplyWallSlide && (Velocity.Z < 0.f))
		{
			return Super::GetGravityZ() * SlideGravityScaling * (1.f - FMath::Square(WallSlideNormal.Z));
		}
		return Super::GetGravityZ();
	}

	float UUTCharacterMovement::SlideAlongSurface(const FVector& Delta, float Time, const FVector& InNormal, FHitResult& Hit, bool bHandleImpact)
	{
		if (Hit.bBlockingHit)
		{
			bSlidingAlongWall = true;
		}
		return Super::SlideAlongSurface(Delta, Time, InNormal, Hit, bHandleImpact);
	}


//
// DODGE
//
	void UUTCharacterMovement::ClearDodgeInput()
	{
		//UE_LOG(UTNet, Warning, TEXT("ClearDodgeInput"));
		bPressedDodgeForward = false;
		bPressedDodgeBack = false;
		bPressedDodgeLeft = false;
		bPressedDodgeRight = false;
		bPressedSlide = false;
	}

	void UUTCharacterMovement::GetDodgeDirection(FVector& OutDodgeDir, FVector& OutDodgeCross) const
	{
		float DodgeDirX = bPressedDodgeForward ? 1.f : (bPressedDodgeBack ? -1.f : 0.f);
		float DodgeDirY = bPressedDodgeLeft ? -1.f : (bPressedDodgeRight ? 1.f : 0.f);
		float DodgeCrossX = (bPressedDodgeLeft || bPressedDodgeRight) ? 1.f : 0.f;
		float DodgeCrossY = (bPressedDodgeForward || bPressedDodgeBack) ? 1.f : 0.f;
		FRotator TurnRot(0.f, CharacterOwner->GetActorRotation().Yaw, 0.f);
		FRotationMatrix TurnRotMatrix = FRotationMatrix(TurnRot);
		FVector X = TurnRotMatrix.GetScaledAxis(EAxis::X);
		FVector Y = TurnRotMatrix.GetScaledAxis(EAxis::Y);
		OutDodgeDir = (DodgeDirX * X + DodgeDirY * Y).GetSafeNormal();
		OutDodgeCross = (DodgeCrossX * X + DodgeCrossY * Y).GetSafeNormal();
	}
	*/
};
