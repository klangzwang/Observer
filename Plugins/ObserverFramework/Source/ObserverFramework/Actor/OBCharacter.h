#pragma once
#include "IOBDamageable.h"
#include "IOBAttacker.h"
#include "IOBEntityInterface.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OBAnimInstance.h"
#include "OBTypes.h"
#include "GenericTeamAgentInterface.h"
/*
#include "Engine/CollisionProfile.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "DrawDebugHelpers.h"
#include "CollisionQueryParams.h"
*/
#include "OBCharacter.generated.h"

class UOBHealthComponent;
class UOBSkeletalMeshComponent;
class AOBPlayerController;
class AOBPlayerState;
//struct FCollisionQueryParams;
//struct FHitResult;

UCLASS(config = Game)
class AOBCharacter : public ACharacter, public IOBAttacker, public IOBDamageable, public IGenericTeamAgentInterface, public IOBEntityInterface
{
	GENERATED_BODY()
	
public:

	AOBCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void BeginAndPlay();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

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
	FTimerHandle RespawnTimerHandle;

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

/*
private:

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
*/

public:

	UFUNCTION()
	void SetIsDetectable(bool bIsDetectable);

	UFUNCTION()
	void SetIsVisible(bool bIsVisible);

private:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Observer|Sound", meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<USoundBase>> SpawnSounds;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Observer|Health", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UOBHealthComponent> HealthComponent;

public:

	UFUNCTION(BlueprintCallable, Category = "Observer|Damage")
	virtual void ApplyDamage(float Damage, AActor* DamageCauser, const FVector& DamageLocation, const FVector& DamageImpulse) override;

	UFUNCTION(BlueprintCallable, Category = "Observer|Damage")
	virtual void AttachProjectile(AActor* DamageCauser, const FVector& DamageLocation, const FVector& DamageImpulse) override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void DoAttackTrace(FName DamageSourceBone) override;

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
};
