#pragma once
#include "IOBDamageable.h"
#include "IOBAttacker.h"
#include "AbilitySystemInterface.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OBAnimInstance.h"
#include "OBTypes.h"
#include "GenericTeamAgentInterface.h"
#include "OBCharacter.generated.h"

class UOBHealthComponent;
class UOBSkeletalMeshComponent;
class AOBPlayerController;
class AOBPlayerState;

class UAbilitySystemComponent;

UCLASS(config = Game)
class AOBCharacter : public ACharacter, public IAbilitySystemInterface, public IOBAttacker, public IOBDamageable, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
public:

	AOBCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void BeginAndPlay();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

public:

	UFUNCTION(BlueprintCallable, Category = "Observer|Character")
	AOBPlayerController* GetOBPlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "Observer|Character")
	AOBPlayerState* GetOBPlayerState() const;

protected:

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, Category = "Observer|Mesh", meta = (AllowPrivateAccess = true))
	UOBSkeletalMeshComponent* MeshComponent;

public:

	UFUNCTION(BlueprintCallable, Category = "Observer|Mesh")
	UOBSkeletalMeshComponent* GetMeshComp() { return MeshComponent; };

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Observer|Character")
	bool bHasTheKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Observer|Character")
	bool bHasTheChest;

private:

	FTimerHandle BeginTimerHandle;

protected:

    UPROPERTY(VisibleAnywhere, Category = "Observer|Character")
    TObjectPtr<class UAIPerceptionStimuliSourceComponent> AIPerceptionStimuliSource;

public:

	virtual float PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None) override;
	virtual void StopAnimMontage(class UAnimMontage* AnimMontage) override;
	void StopAllAnimMontages(float blendout);

	UOBAnimInstance* GetAnimInstance() { return Cast<UOBAnimInstance>(GetMesh()->AnimScriptInstance); };

public:

	UFUNCTION(BlueprintCallable, Category = "Observer|Character")
	void DisableMovementAndCollision();

protected:

	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

	UFUNCTION(BlueprintCallable, Category = "Observer|Character")
	void UninitAndDestroy();

public:

	UFUNCTION()
	void SetIsDetectable(bool bIsDetectable);

	UFUNCTION()
	void SetIsVisible(bool bIsVisible);

private:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Observer|Sound", meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<USoundBase>> SpawnSounds;

public:

    UFUNCTION(BlueprintCallable, Category = "Observer|Character")
    void AssignTeam(ETeam team);

    UFUNCTION(BlueprintPure, Category = "Observer|Character")
    ETeam GetCombatTeam() const;
	
	UPROPERTY()
    ETeam CombatTeam = ETeam::ETeam1;

public:

    virtual void SetGenericTeamId(const FGenericTeamId& NewTeamId) override;
    virtual FGenericTeamId GetGenericTeamId() const override;

protected:

    UPROPERTY(EditAnywhere, Category = "Observer|Character")
    FGenericTeamId GenericTeamId;

/*
//
// Interaction
//
protected:

	static void LineTrace(FHitResult& OutHitResult, const UWorld* World, const FVector& Start, const FVector& End,
		const ECollisionChannel InChannel, const FCollisionQueryParams Params);

public:

	UFUNCTION(BlueprintCallable, Category = "Observer|Character")
	void PerformInteractionCheck();

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Observer|Character")
	float InteractionRange = 1000.f;
*/

//
// Health
//
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Observer|Health", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UOBHealthComponent> HealthComponent;

//
// Attack and Damage
//
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

public:

	virtual void DoAttackTrace(FName DamageSourceBone) override;

	UFUNCTION(BlueprintCallable, Category = "Observer|Damage")
	virtual void ApplyDamage(float Damage, AActor* DamageCauser, const FVector& DamageLocation, const FVector& DamageImpulse) override;

public:

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "Observer|Damage", meta = (DisplayName = "OnDealtDamage", ScriptName = "OnDealtDamage"))
	void K2_OnDealtDamage(float Damage, const FVector& ImpactPoint);

	UFUNCTION(BlueprintImplementableEvent, Category = "Observer|Damage", meta = (DisplayName = "OnReceivedDamage", ScriptName = "OnReceivedDamage"))
	void K2_OnReceivedDamage(float Damage, const FVector& ImpactPoint, const FVector& DamageDirection);

//
// Death and Respawning
//
protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "Observer|Death", meta = (DisplayName = "OnDeathStarted", ScriptName = "OnDeathStarted"))
	void K2_OnDeathStarted();

	UFUNCTION(BlueprintImplementableEvent, Category = "Observer|Death", meta = (DisplayName = "OnDeathFinished", ScriptName = "OnDeathFinished"))
	void K2_OnDeathFinished();

private:

	FTimerHandle RespawnTimerHandle;

public:

	virtual void HandleDeath() override;
};
