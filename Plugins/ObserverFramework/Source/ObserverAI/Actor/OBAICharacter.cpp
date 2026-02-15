#include "OBAICharacter.h"
#include "OBAIController.h"
//#include "OBHealthComponent.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/CharacterMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBAICharacter)

static FName NAME_AICharacterCollisionProfile_Capsule(TEXT("BlockThePlayer"));

AOBAICharacter::AOBAICharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	AIControllerClass = AOBAIController::StaticClass();

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->InitCapsuleSize(40.0f, 135.0f);
	CapsuleComp->SetCollisionProfileName(NAME_AICharacterCollisionProfile_Capsule);

	GetMesh()->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.f, 0.0f));
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -135.f));

	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	MoveComp->bOrientRotationToMovement = true;
	MoveComp->MaxWalkSpeed = 200.f;
	MoveComp->RotationRate = FRotator(0.f, 48.f, 0.f);

	HealthComponent = CreateDefaultSubobject<UOBHealthComponent>(TEXT("HealthComponent"));
}

void AOBAICharacter::BeginPlay()
{
	if (HealthComponent)
		HealthComponent->InitHealth();

	Super::BeginPlay();
}

void AOBAICharacter::DoAttackTrace(FName DamageSourceBone)
{
	// sweep for objects in front of the character to be hit by the attack
	TArray<FHitResult> OutHits;

	// start at the provided socket location, sweep forward
	const FVector TraceStart = GetMesh()->GetSocketLocation(DamageSourceBone);
	const FVector TraceEnd = TraceStart + (GetActorForwardVector() * MeleeTraceDistance);

	// check for pawn and world dynamic collision object types
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
	ObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	// use a sphere shape for the sweep
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(MeleeTraceRadius);

	// ignore self
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->SweepMultiByObjectType(OutHits, TraceStart, TraceEnd, FQuat::Identity, ObjectParams, CollisionShape, QueryParams))
	{
		// iterate over each object hit
		for (const FHitResult& CurrentHit : OutHits)
		{
			// check if we've hit a damageable actor
			IOBDamageable* Damageable = Cast<IOBDamageable>(CurrentHit.GetActor());

			if (Damageable)
			{
				// knock upwards and away from the impact normal
				const FVector Impulse = (CurrentHit.ImpactNormal * -MeleeKnockbackImpulse) + (FVector::UpVector * MeleeLaunchImpulse);

				// pass the damage event to the actor
				Damageable->ApplyDamage(MeleeDamage, this, CurrentHit.ImpactPoint, Impulse);

				K2_OnDealtDamage(MeleeDamage, CurrentHit.ImpactPoint);
			}
		}
	}
}

void AOBAICharacter::ApplyDamage(float Damage, AActor* DamageCauser, const FVector& DamageLocation, const FVector& DamageImpulse)
{
	GetCharacterMovement()->AddImpulse(DamageImpulse, true);

	if (GetMesh()->IsSimulatingPhysics())
		GetMesh()->AddImpulseAtLocation(DamageImpulse * GetMesh()->GetMass(), DamageLocation);

	K2_OnReceivedDamage(Damage, DamageLocation, DamageImpulse.GetSafeNormal());

	float CurrentHP = HealthComponent->GetHealth();

	CurrentHP -= Damage;
	HealthComponent->SetHealth(CurrentHP);
}

float AOBAICharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float CurrentHP = HealthComponent->GetHealth();

	if (CurrentHP <= 0.0f)
		return 0.0f;

	CurrentHP -= Damage;
	HealthComponent->SetHealth(CurrentHP);

	if (CurrentHP <= 0.0f)
	{
		if (GetController())
		{
			GetController()->SetIgnoreMoveInput(true);
		}

		UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
		check(CapsuleComp);
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

		UCharacterMovementComponent* MoveComp = GetCharacterMovement();
		MoveComp->StopMovementImmediately();
		MoveComp->DisableMovement();

		GetMesh()->SetSimulatePhysics(true);
	}
	else
	{
		GetMesh()->SetPhysicsBlendWeight(0.5f);
		GetMesh()->SetBodySimulatePhysics("pelvis", false);
	}
	return Damage;
}

void AOBAICharacter::AssignTeam(ETeam team)
{
	CombatTeam = team;
	SetGenericTeamId(uint8(CombatTeam));
}

ETeam AOBAICharacter::GetCombatTeam() const
{
	return CombatTeam;
}

void AOBAICharacter::SetGenericTeamId(const FGenericTeamId& InTeamID)
{
	CombatTeam = (ETeam)InTeamID.GetId();
}

FGenericTeamId AOBAICharacter::GetGenericTeamId() const
{
	return uint8(CombatTeam);
}

float AOBAICharacter::PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName)
{
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		if (AnimMontage && AnimInstance)
		{
			if (!AnimInstance->Montage_IsPlaying(AnimMontage))
			{
				return AnimInstance->Montage_Play(AnimMontage, InPlayRate);
			}
			else
			{
				if (StartSectionName != NAME_None && AnimInstance->Montage_GetCurrentSection(AnimMontage) != StartSectionName)
				{
					AnimInstance->Montage_JumpToSection(StartSectionName, AnimMontage);
				}
			}
		}
	}
	return 0.0f;
}

void AOBAICharacter::StopAnimMontage(class UAnimMontage* AnimMontage)
{
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		if (AnimMontage && AnimInstance && AnimInstance->Montage_IsPlaying(AnimMontage))
			AnimInstance->Montage_Stop(AnimMontage->BlendOut.GetBlendTime());
	}
}

void AOBAICharacter::StopAllAnimMontages(float blendout)
{
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		if (AnimInstance)
			AnimInstance->Montage_Stop(blendout);
	}
}
