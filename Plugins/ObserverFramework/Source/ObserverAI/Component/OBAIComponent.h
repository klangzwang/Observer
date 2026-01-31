#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKey.h"
#include "BehaviorTree/BehaviorTree.h"
#include "OBTypes.h"
#include "OBAIComponent.generated.h"

class UOBAIStorageComponent;
class AAIController;
class ACharacter;
class AActor;
class ATargetPoint;
class UObject;

UCLASS(MinimalAPI, Blueprintable, Meta = (BlueprintSpawnableComponent))
class UOBAIComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UOBAIComponent();

public:

	UFUNCTION(BlueprintPure, Category = "Observer|AIComponent")
	static UOBAIComponent* FindAIComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UOBAIComponent>() : nullptr); }

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Observer|AIComponent", meta = (DisplayName = "OnTargetPerceived", ScriptName = "OnTargetPerceived"))
	void K2_OnTargetPerceived();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Observer|AIComponent", meta = (DisplayName = "OnTargetLost", ScriptName = "OnTargetLost"))
	void K2_OnTargetLost();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Observer|AIComponent", meta = (DisplayName = "OnTargetForgotten", ScriptName = "OnTargetForgotten"))
	void K2_OnTargetForgotten();

protected:

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Observer|AIComponent", meta = (DisplayName = "TrySearchingPatrolPoint", ScriptName = "TrySearchingPatrolPoint"))
	void K2_TrySearchingPatrolPoint();

public:

	UFUNCTION(BlueprintPure, Category = "Getter")
	FVector GetTargetLocation();

	UFUNCTION(BlueprintPure, Category = "Getter")
	FVector GetTargetLastLocation();

	UFUNCTION(BlueprintPure, Category = "Getter")
	AActor* GetEQSContextActor();

	UFUNCTION(BlueprintPure, Category = "Getter")
	AActor* GetTargetActor();

	UFUNCTION(BlueprintPure, Category = "Getter")
	FBehaviorBasic& GetCurrentBehaviorConfig();

	UFUNCTION(BlueprintPure, Category = "Getter")
	ACharacter* GetOwningCharacter();

	UFUNCTION(BlueprintPure, Category = "Getter")
	EBehaviorType& GetCurrentBehaviorType();

	UFUNCTION(BlueprintPure, Category = "Getter")
	TArray<AActor*> GetAIStorageActors();

public:

	UFUNCTION(BlueprintCallable, Category = "Setters")
	void SetTargetActor(AActor* NewTarget);

	UFUNCTION(BlueprintCallable, Category = "Setters")
	void SetTargetLocation(FVector NewLocation);

	UFUNCTION(BlueprintCallable, Category = "Setters")
	void SetTargetLastLocation(FVector NewLocation);

/*
private:

	void SetControlRotation(AActor* InLookToActor);
*/

public:

	UFUNCTION(BlueprintPure, Category = "Basic")
	bool CheckIfAliveIfNPC(AActor* ActorRef);

	UFUNCTION(BlueprintCallable, Category = "Basic")
	void ChangeBehavior(EBehaviorType InBehaviorType);

	UFUNCTION(BlueprintCallable, Category = "Basic")
	void CheckForWithinDistanceTriggers();

	UFUNCTION(BlueprintCallable, Category = "Basic")
	void CheckForBeyondDistanceTriggers();

public:

	UFUNCTION(BlueprintCallable, Category = "Finders")
	bool TryGetNextWaypoint(FVector& outLocation);

	UFUNCTION(BlueprintCallable, Category = "Finders")
	void UpdatePatrolLocation(FVector& outLocation);

	UFUNCTION(BlueprintCallable, Category = "Finders")
	void UpdatePatrolLocationOnSpline(FVector& outLocation);

public:

	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DebugRenderContextActor();

private:

	UPROPERTY(EditDefaultsOnly, Category = "Observer|Behavior|GeneralSettings")
	EBehaviorType PrevBehaviorType;

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Observer|Behavior|GeneralSettings", meta = (ExposeOnSpawn = "true"))
	EBehaviorType InitialBehaviorType;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Observer|Behavior|GeneralSettings")
	EBehaviorType CurrentBehaviorType;

	UPROPERTY(EditDefaultsOnly, Category = "Observer|Behavior|GeneralSettings")
	FVector SenseLocation;


public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Observer|Behavior|GeneralSettings")
	bool IsAlive = true;

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Observer|AIComponent|Behavior|Patrol")
	EPatrolType PatrolType;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Observer|AIComponent|Behavior|Patrol")
	int32 PatrolSplineIndex;

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Observer|Behavior")
	TObjectPtr<ACharacter> AIOwner;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Observer|Behavior")
	TObjectPtr<AAIController> AIController;

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Observer|AIComponent|Behavior|Patrol", meta = (ExposeOnSpawn = "true"))
	TObjectPtr<AOBAIPatrolPath> AIPatrolPath;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Observer|Behavior")
	TObjectPtr<ATargetPoint> AIContextActor;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Observer|Behavior")
	TObjectPtr<UAIPerceptionComponent> AIPerception;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Observer|Behavior", meta = (ExposeOnSpawn = "true"))
	TObjectPtr<UBehaviorTree> BehaviorTree;

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Observer|AIComponent")
	ACharacter* GetAIOwner() const { return AIOwner; };

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Observer|AIComponent")
    AAIController* GetAIController() const { return AIController; };

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Observer|AIComponent")
	ATargetPoint* GetContextActor() const { return AIContextActor; };

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Observer|AIComponent")
    AOBAIPatrolPath* GetAIPatrolPath() const { return AIPatrolPath; };
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Observer|AIComponent")
    UAIPerceptionComponent* GetPerceptionComponent() const { return AIPerception; };

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true, ExposeOnSpawn = true), Category = "Observer|AIComponent|Behavior")
	FBehaviorBasic CurrentBehaviorConfig = FBehaviorBasic();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true, ExposeOnSpawn = true), Category = "Observer|AIComponent|Behavior")
	FBehaviorBasic Idle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true, ExposeOnSpawn = true), Category = "Observer|AIComponent|Behavior")
	FBehaviorBasic Follow;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true, ExposeOnSpawn = true), Category = "Observer|AIComponent|Behavior")
	FBehaviorBasic Patrol;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true, ExposeOnSpawn = true), Category = "Observer|AIComponent|Behavior")
	FBehaviorBasic AttackMelee;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true, ExposeOnSpawn = true), Category = "Observer|AIComponent|Behavior")
	FBehaviorBasic Investigate;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Observer|AIComponent|Behavior")
	TArray<FBehaviorBasic> BehaviorArray;

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBeginIdle);
	UPROPERTY(BlueprintAssignable, EditDefaultsOnly, Category = "IdleSettings")
	FOnBeginIdle OnBeginIdle;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndIdle);
	UPROPERTY(BlueprintAssignable, EditDefaultsOnly, Category = "IdleSettings")
	FOnEndIdle OnEndIdle;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBeginFollow);
	UPROPERTY(BlueprintAssignable, EditDefaultsOnly, Category = "FollowSettings")
	FOnBeginFollow OnBeginFollow;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndFollow);
	UPROPERTY(BlueprintAssignable, EditDefaultsOnly, Category = "FollowSettings")
	FOnEndFollow OnEndFollow;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBeginPatrol);
	UPROPERTY(BlueprintAssignable, EditDefaultsOnly, Category = "PatrolSettings")
	FOnBeginPatrol OnBeginPatrol;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndPatrol);
	UPROPERTY(BlueprintAssignable, EditDefaultsOnly, Category = "PatrolSettings")
	FOnEndPatrol OnEndPatrol;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBeginAttackMelee);
	UPROPERTY(BlueprintAssignable, EditDefaultsOnly, Category = "AttackSettings")
	FOnBeginAttackMelee OnBeginAttackMelee;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndAttackMelee);
	UPROPERTY(BlueprintAssignable, EditDefaultsOnly, Category = "AttackSettings")
	FOnEndAttackMelee OnEndAttackMelee;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBeginInvestigate);
	UPROPERTY(BlueprintAssignable, EditDefaultsOnly, Category = "InvestigateSettings")
	FOnBeginInvestigate OnBeginInvestigate;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndInvestigate);
	UPROPERTY(BlueprintAssignable, EditDefaultsOnly, Category = "InvestigateSettings")
	FOnEndInvestigate OnEndInvestigate;
};
