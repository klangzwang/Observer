#include "OBCharacter.h"
#include "OBHealthComponent.h"
#include "OBPlayerController.h"
#include "OBPlayerState.h"
#include "OBHeroComponent.h"
#include "OBCharacterMovementComponent.h"
#include "OBSkeletalMeshComponent.h"
#include "OBStealth.h"
#include "IOBInteractable.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "GenericTeamAgentInterface.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Camera/CameraComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBCharacter)

static FName NAME_CharacterCollisionProfile_Capsule(TEXT("BlockTheEnemy"));

AOBCharacter::AOBCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer
	.SetDefaultSubobjectClass<UOBCharacterMovementComponent>(ACharacter::CharacterMovementComponentName)
	.SetDefaultSubobjectClass<UOBSkeletalMeshComponent>(ACharacter::MeshComponentName)
	)
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->InitCapsuleSize(40.0f, 90.0f);
	CapsuleComp->SetCollisionProfileName(NAME_CharacterCollisionProfile_Capsule);

	UOBSkeletalMeshComponent* OBMesh = CastChecked<UOBSkeletalMeshComponent>(GetMesh());
	OBMesh->SetRelativeRotation(FRotator(0.0f, -90.f, 0.0f));
	OBMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -90.f));
	//OBMesh->OnComponentBeginOverlap.AddDynamic(this, &AOBCharacter::OnOverlapBegin);
	//OBMesh->OnComponentEndOverlap.AddDynamic(this, &AOBCharacter::OnOverlapEnd);

	UOBCharacterMovementComponent* OBMoveComp = CastChecked<UOBCharacterMovementComponent>(GetCharacterMovement());
	OBMoveComp->bAllowPhysicsRotationDuringAnimRootMotion = false;
	OBMoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;
	OBMoveComp->bCanWalkOffLedgesWhenCrouching = true;

	OBMoveComp->bUseControllerDesiredRotation = false;
	OBMoveComp->bOrientRotationToMovement = false;

	OBMoveComp->RotationRate = FRotator(0.f, 0.f, 0.f);

	OBMoveComp->SetCrouchedHalfHeight(60.0f);

	OBMoveComp->SetWalkableFloorAngle(50.f);
	OBMoveComp->GroundFriction = 6.f;
	OBMoveComp->MaxWalkSpeed = 450.f;
	OBMoveComp->BrakingDecelerationWalking = 800.f;

	OBMoveComp->GravityScale = 1.6f;
	OBMoveComp->MaxAcceleration = 1000.0f;
	OBMoveComp->BrakingFrictionFactor = 0.f;
	OBMoveComp->BrakingFriction = 6.0f;
	OBMoveComp->GroundFriction = 6.0f;
	OBMoveComp->BrakingDecelerationWalking = 800.0f;

	OBMoveComp->bTouchForceScaledToMass = false;
	OBMoveComp->bScalePushForceToVelocity = false;
	OBMoveComp->InitialPushForceFactor = 0.f;
	OBMoveComp->PushForceFactor = 0.f;
	OBMoveComp->bAllowPhysicsRotationDuringAnimRootMotion = true;

	HealthComponent = CreateDefaultSubobject<UOBHealthComponent>(TEXT("HealthComponent"));

	AIPerceptionStimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("PerceptionStimuliComponent"));
	AIPerceptionStimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
	AIPerceptionStimuliSource->RegisterForSense(UAISense_Hearing::StaticClass());
	AIPerceptionStimuliSource->RegisterWithPerceptionSystem();

	PrimaryActorTick.bStartWithTickEnabled = false;
	CombatTeam = ETeam::ETeam1;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	BaseEyeHeight = 80.0f;
	CrouchedEyeHeight = 50.0f;

	bHasTheKey = false;
	bHasTheChest = false;

	AIControllerClass = nullptr;

	Tags.Add(FName("Player"));
}

void AOBCharacter::BeginPlay()
{
	if (HealthComponent)
		HealthComponent->InitHealth();

	Super::BeginPlay();

	/*
	GetWorld()->GetTimerManager().SetTimer(
		BeginTimerHandle,
		this,
		&ThisClass::BeginAndPlay,
		0.5f,
		true,
		0.5f);
	*/
}

void AOBCharacter::BeginAndPlay()
{
	/*
	if (AOBWorldSettings* WorldSettings = Cast<AOBWorldSettings>(GetWorld()->GetWorldSettings()))
	{
		if (AOBGameState* GameState = Cast<AOBGameState>(GetWorld()->GetGameState()))
		{
			if (ensure(WorldSettings) && ensure(GameState))
			{
				float CameraWaitTimer = 0.5f;

				if (WorldSettings->GetWorldSetup().PlayIntroCamSequence)
				{
					CameraWaitTimer = 5.0f;
					//GameState->K2_OnCameraIntro();
				}
			}
		}
	}
	*/

	if (UOBHeroComponent* Hero = FindComponentByClass<UOBHeroComponent>())
	{
		if (ensure(Hero))
		{
			if (Hero->GetCharacterClass().bIsCrouchingAtStart)
				Crouch();

			/*
			if (Hero->GetCharacterClass().mWeapon)
			{
				Weapon = Cast<AOBWeapon>(GetWorld()->SpawnActorDeferred<AActor>(Hero->GetCharacterClass().mWeapon, FTransform::Identity, this));
				Weapon->FinishSpawning(FTransform::Identity, true);

				if (Weapon != nullptr)
				{
					Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "weapon_r");
					Weapon->SetWeaponOwner(this);

					PlayAnimMontage(Weapon->WeaponDef.EquipMontage);
					GetMesh()->LinkAnimClassLayers(Weapon->WeaponDef.AnimLayer);
				}
			}
			*/

			if (Hero->GetCharacterClass().SpawnSound)
			{
				UGameplayStatics::SpawnSoundAttached(Hero->GetCharacterClass().SpawnSound, RootComponent, FName("None"), FVector::ZeroVector, FRotator::ZeroRotator,
					EAttachLocation::SnapToTarget, false, 1.f, 1.f, 0.0f, nullptr, nullptr, true);
			}
		}
	}

	GetWorld()->GetTimerManager().ClearTimer(BeginTimerHandle);

	Super::BeginPlay();
}

void AOBCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle);
	//GetWorld()->GetTimerManager().ClearTimer(BeginTimerHandle);
}

AOBPlayerController* AOBCharacter::GetOBPlayerController() const
{
	return CastChecked<AOBPlayerController>(GetController(), ECastCheckedType::NullAllowed);
}

AOBPlayerState* AOBCharacter::GetOBPlayerState() const
{
	return CastChecked<AOBPlayerState>(GetPlayerState(), ECastCheckedType::NullAllowed);
}

void AOBCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UOBHeroComponent* HeroComp = FindComponentByClass<UOBHeroComponent>())
		HeroComp->InitializePlayerInput(PlayerInputComponent);
}

void AOBCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void AOBCharacter::UnPossessed()
{
	if (IsValid(GetController<APlayerController>()))
	{
		if (const ULocalPlayer* LocalPlayer = GetController<APlayerController>()->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				Subsystem->ClearAllMappings();
			}
		}
	}

	Super::UnPossessed();
}

float AOBCharacter::PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName)
{
	if (AnimMontage && GetAnimInstance())
	{
		if (!GetAnimInstance()->Montage_IsPlaying(AnimMontage))
		{
			return GetAnimInstance()->Montage_Play(AnimMontage, InPlayRate);
		}
		else
		{
			if (StartSectionName != NAME_None && GetAnimInstance()->Montage_GetCurrentSection(AnimMontage) != StartSectionName)
			{
				GetAnimInstance()->Montage_JumpToSection(StartSectionName, AnimMontage);
			}
		}
	}
	return 0.0f;
}

void AOBCharacter::StopAnimMontage(class UAnimMontage* AnimMontage)
{
	if (AnimMontage && GetAnimInstance() && GetAnimInstance()->Montage_IsPlaying(AnimMontage))
		GetAnimInstance()->Montage_Stop(AnimMontage->BlendOut.GetBlendTime());
}

void AOBCharacter::StopAllAnimMontages(float blendout)
{
	if (GetAnimInstance())
		GetAnimInstance()->Montage_Stop(blendout);
}

void AOBCharacter::DoAttackTrace(FName DamageSourceBone)
{
	/*
	IOBDamageable* Damageable = Cast<IOBDamageable>(CurrentHit.GetActor());
	if (Damageable)
	{
		// knock upwards and away from the impact normal
		const FVector Impulse = (CurrentHit.ImpactNormal * -MeleeKnockbackImpulse) + (FVector::UpVector * MeleeLaunchImpulse);

		// pass the damage event to the actor
		Damageable->ApplyDamage(MeleeDamage, this, CurrentHit.ImpactPoint, Impulse);

		// call the BP handler to play effects, etc.
		K2_OnDealtDamage(MeleeDamage, CurrentHit.ImpactPoint);
	}
	*/
}

/*
void AOBCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<AOBStealth>(OtherActor))
	{
		Tags.Empty();
		Tags.Add(FName("CanBeHidden"));
	}
}

void AOBCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<AOBStealth>(OtherActor))
	{
		Tags.Empty();
		Tags.Add(FName("Player"));
	}
}
*/

void AOBCharacter::SetIsDetectable(bool bIsDetectable)
{
	if (AIPerceptionStimuliSource)
	{
		if (bIsDetectable)
			AIPerceptionStimuliSource->RegisterWithPerceptionSystem();
		else
			AIPerceptionStimuliSource->UnregisterFromPerceptionSystem();
	}
}

void AOBCharacter::SetIsVisible(bool bIsVisible)
{
	if (AIPerceptionStimuliSource)
	{
		if (bIsVisible)
			AIPerceptionStimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
		else
			AIPerceptionStimuliSource->UnregisterFromSense(UAISense_Sight::StaticClass());
		AIPerceptionStimuliSource->RegisterWithPerceptionSystem();
	}
}

/*
void AOBCharacter::ShowOutlines(bool bActive)
{
	TArray<USceneComponent*> AllComps;
	GetMesh()->GetChildrenComponents(true, AllComps);

	if (AllComps.Num() == 0)
		return;

	for (USceneComponent* Comp : AllComps)
	{
		if (Comp->ComponentTags.Num() > 0)
		{
			if (Comp->ComponentTags[0] == "Outline")
			{
				Cast<UPrimitiveComponent>(Comp)->SetRenderCustomDepth(bActive);
			}
		}
	}
}
*/

void AOBCharacter::ApplyDamage(float Damage, AActor* DamageCauser, const FVector& DamageLocation, const FVector& DamageImpulse)
{
	/*
	GetCharacterMovement()->AddImpulse(DamageImpulse, true);

	if (GetMesh()->IsSimulatingPhysics())
		GetMesh()->AddImpulseAtLocation(DamageImpulse * GetMesh()->GetMass(), DamageLocation);

	K2_OnReceivedDamage(Damage, DamageLocation, DamageImpulse.GetSafeNormal());

	if (UOBHeroComponent* Hero = FindComponentByClass<UOBHeroComponent>())
	{
		if (ensure(Hero))
		{
			if (Hero->GetCharacterClass().HitSound)
			{
				//UGameplayStatics::SpawnSoundAttached(Hero->GetCharacterClass().HitSound, GetMesh(), "none", FVector::ZeroVector, FRotator::ZeroRotator,
				//	EAttachLocation::SnapToTarget, false, 1.f, 1.f, 0.0f, nullptr, nullptr, true);
			}
		}
	}

	float CurrentHP = HealthComponent->GetHealth();

	CurrentHP -= Damage;
	HealthComponent->SetHealth(CurrentHP);

	if (CurrentHP <= 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(RespawnTimer, this, &AOBCharacter::UninitAndDestroy, 3.f, false);
		K2_OnDeathStarted();
		return;
	}
	*/
}

float AOBCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	/*
	float CurrentHP = HealthComponent->GetHealth();

	if (CurrentHP <= 0.0f)
		return 0.0f;

	CurrentHP -= Damage;
	HealthComponent->SetHealth(CurrentHP);

	if (CurrentHP <= 0.0f)
	{
		DisableMovementAndCollision();
		GetMesh()->SetSimulatePhysics(true);

		GetWorld()->GetTimerManager().SetTimer(RespawnTimer, this, &AOBCharacter::UninitAndDestroy, 3.f, false);

		K2_OnDeathStarted();
	}
	else
	{
		GetMesh()->SetPhysicsBlendWeight(0.5f);
		GetMesh()->SetBodySimulatePhysics("pelvis", false);
	}
	return Damage;
	*/

	return Damage;
}

void AOBCharacter::AttachProjectile(AActor* DamageCauser, const FVector& DamageLocation, const FVector& DamageImpulse)
{
	//	if(GetWeapon()->WeaponDef.CanAttachProjectile)
	//		DamageCauser->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform);
}

void AOBCharacter::FellOutOfWorld(const class UDamageType& dmgType)
{
	HealthComponent->DamageSelfDestruct(true);
}

void AOBCharacter::DisableMovementAndCollision()
{
	if (GetController())
	{
		GetController()->SetIgnoreMoveInput(true);
	}

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	UOBCharacterMovementComponent* OBMoveComp = CastChecked<UOBCharacterMovementComponent>(GetCharacterMovement());
	OBMoveComp->StopMovementImmediately();
	OBMoveComp->DisableMovement();
}

void AOBCharacter::UninitAndDestroy()
{
	/*
	if (GetLocalRole() == ROLE_Authority)
	{
		DetachFromControllerPendingDestroy();
		SetLifeSpan(0.1f);
	}

	SetActorHiddenInGame(true);

	if (AOBGameMode* GameMode = Cast<AOBGameMode>(UGameplayStatics::GetGameMode(this)))
		GameMode->RestartPlayer(Controller);

	K2_OnDeathFinished();
	*/
}

void AOBCharacter::AssignTeam(ETeam team)
{
	CombatTeam = team;
	SetGenericTeamId(uint8(CombatTeam));
}

ETeam AOBCharacter::GetCombatTeam() const
{
	return CombatTeam;
}

void AOBCharacter::SetGenericTeamId(const FGenericTeamId& InTeamID)
{
	CombatTeam = (ETeam)InTeamID.GetId();
}

FGenericTeamId AOBCharacter::GetGenericTeamId() const
{
	return uint8(CombatTeam);
}

void AOBCharacter::PerformInteractionCheck()
{
	FVector ViewStart;
	FRotator ViewRot;

	GetOBPlayerController()->GetPlayerViewPoint(ViewStart, ViewRot);

	const FVector ViewDir = ViewRot.Vector();
	FVector ViewEnd = ViewStart + (ViewDir * InteractionRange);

	FVector CameraToCenter = GetTransform().GetLocation() - ViewStart;
	float DotToCenter = FVector::DotProduct(CameraToCenter, ViewDir);
	if (DotToCenter >= 0)
	{
		float DistanceSquared = CameraToCenter.SizeSquared() - (DotToCenter * DotToCenter);
		float RadiusSquared = (InteractionRange * InteractionRange);
		if (DistanceSquared <= RadiusSquared)
		{
			float DistanceFromCamera = FMath::Sqrt(RadiusSquared - DistanceSquared);
			float DistanceAlongRay = DotToCenter + DistanceFromCamera;
			ViewEnd = ViewStart + (DistanceAlongRay * ViewDir);
		}
	}

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	LineTrace(HitResult, GetWorld(), ViewStart, ViewEnd, ECC_Visibility, Params);

	DrawDebugLine(GetWorld(), HitResult.TraceStart, HitResult.TraceEnd, FColor::Green, false);
	DrawDebugSphere(GetWorld(), HitResult.TraceEnd, 16, 10, FColor::Green, false);

	if (HitResult.bBlockingHit)
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor && HitActor->Implements<UOBInteractable>())
		{
			// FInteractionData Data = IOBInteractable::Execute_GetInteractionData(HitActor);
			IOBInteractable::Execute_OnInteract(HitActor, this);
		}
	}
}

void AOBCharacter::LineTrace(FHitResult& OutHitResult, const UWorld* World, const FVector& Start, const FVector& End,
	const ECollisionChannel InChannel, const FCollisionQueryParams Params)
{
	check(World);

	OutHitResult = FHitResult();
	FHitResult HitResult;
	World->LineTraceSingleByChannel(HitResult, Start, End, InChannel, Params);

	OutHitResult.TraceStart = Start;
	OutHitResult.TraceEnd = End;

	if (HitResult.bBlockingHit)
	{
		OutHitResult = HitResult;
	}
}