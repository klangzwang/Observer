#pragma once
#include "IOBDamageable.h"
#include "IOBAttacker.h"
//#include "IOBEntityInterface.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OBTypes.h"
#include "OBHealthComponent.h"
#include "GenericTeamAgentInterface.h"
#include "OBAICharacter.generated.h"

//class UOBHealthComponent;
class UAnimInstance;
class UAnimMontage;

UCLASS(config = Game)
class AOBAICharacter : public ACharacter, public IOBAttacker, public IOBDamageable, public IGenericTeamAgentInterface //, public IOBEntityInterface
{
	GENERATED_BODY()
	
public:

	AOBAICharacter();

	virtual void BeginPlay() override;

	virtual float PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None) override;
	virtual void StopAnimMontage(class UAnimMontage* AnimMontage) override;
	void StopAllAnimMontages(float blendout);

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Observer|Health", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UOBHealthComponent> HealthComponent;

public:

	UFUNCTION(BlueprintCallable, Category = "Observer|Damage")
	virtual void ApplyDamage(float Damage, AActor* DamageCauser, const FVector& DamageLocation, const FVector& DamageImpulse) override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void DoAttackTrace(FName DamageSourceBone) override;

protected:

	/** Distance ahead of the character that melee attack sphere collision traces will extend */
	UPROPERTY(EditAnywhere, Category = "Observer|Trace", meta = (ClampMin = 0, ClampMax = 500, Units = "cm"))
	float MeleeTraceDistance = 75.0f;

	/** Radius of the sphere trace for melee attacks */
	UPROPERTY(EditAnywhere, Category = "Observer|Trace", meta = (ClampMin = 0, ClampMax = 200, Units = "cm"))
	float MeleeTraceRadius = 75.0f;

	/** Amount of damage a melee attack will deal */
	UPROPERTY(EditAnywhere, Category = "Observer|Damage", meta = (ClampMin = 0, ClampMax = 100))
	float MeleeDamage = 1.0f;

	/** Amount of knockback impulse a melee attack will apply */
	UPROPERTY(EditAnywhere, Category = "Observer|Damage", meta = (ClampMin = 0, ClampMax = 1000, Units = "cm/s"))
	float MeleeKnockbackImpulse = 250.0f;

	/** Amount of upwards impulse a melee attack will apply */
	UPROPERTY(EditAnywhere, Category = "Observer|Damage", meta = (ClampMin = 0, ClampMax = 1000, Units = "cm/s"))
	float MeleeLaunchImpulse = 300.0f;

protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "Observer|Damage", meta = (DisplayName = "OnDealtDamage", ScriptName = "OnDealtDamage"))
	void K2_OnDealtDamage(float Damage, const FVector& ImpactPoint);

	UFUNCTION(BlueprintImplementableEvent, Category = "Observer|Damage", meta = (DisplayName = "OnReceivedDamage", ScriptName = "OnReceivedDamage"))
	void K2_OnReceivedDamage(float Damage, const FVector& ImpactPoint, const FVector& DamageDirection);

public:

	UFUNCTION(BlueprintCallable, Category = "Observer|AICharacter")
	void AssignTeam(ETeam team);

	UFUNCTION(BlueprintPure, Category = "Observer|AICharacter")
	ETeam GetCombatTeam() const;

	UPROPERTY()
	ETeam CombatTeam = ETeam::ETeam2;

public:

	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamId) override;
	virtual FGenericTeamId GetGenericTeamId() const override;

protected:

	UPROPERTY(EditAnywhere, Category = "Observer|AICharacter")
	FGenericTeamId GenericTeamId;
};
