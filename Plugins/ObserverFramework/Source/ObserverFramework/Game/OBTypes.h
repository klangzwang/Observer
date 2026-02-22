#pragma once
#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Sound/SoundBase.h"
#include "Animation/AnimMontage.h"
#include "GameplayTagContainer.h"
#include "Engine/Engine.h"
#include "OBTypes.generated.h"

class UCommonActivatableWidget;

UENUM(BlueprintType)
enum class EInteractionResult : uint8
{
	Completed,
	Canceled
};

UENUM(BlueprintType)
enum class EInteractionColliderArt : uint8
{
	Sphere,
	Box
};

UENUM(BlueprintType)
enum class EFootSideType : uint8
{
    None,
    Left,
    Right
};

UENUM(BlueprintType)
enum class ETeam : uint8
{
    ETeam1 UMETA(DisplayName = "Team 1"),
    ETeam2 UMETA(DisplayName = "Team 2"),
    ENeutral UMETA(DisplayName = "Neutral")
};

UENUM(BlueprintType, Blueprintable)
enum class EBehaviorType : uint8
{
    Idle,
    Follow,
	Patrol,
	AttackMelee,
	Investigate
};

UENUM(BlueprintType, Blueprintable)
enum class EIdleArt : uint8
{
	Chilled,
	ReturnHome,
	ReturnPatrol,
	LookAround,
	Alerted
};

UENUM(BlueprintType, Blueprintable)
enum class EFollowArt : uint8
{
	Standard,
	Distanced,
	NoDistance
};

UENUM(BlueprintType, Blueprintable)
enum class EPatrolArt : uint8
{
	Spline,
	Target
};

UENUM(BlueprintType, Blueprintable)
enum class EPatrolType : uint8
{
	Single,
	Looping,
	BackAndForth
};

//
//
//
USTRUCT(BlueprintType)
struct FOBHUDLayoutRequest
{
	GENERATED_BODY()

public:

	OBSERVERFRAMEWORK_API FOBHUDLayoutRequest();

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Categories = "UI.Layer", AllowPrivateAccess = true))
	FGameplayTag LayerID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AssetBundles = "Client", AllowPrivateAccess = true))
	TSoftClassPtr<UCommonActivatableWidget> LayoutClass;
};

USTRUCT(BlueprintType)
struct FActorTrigger
{
	GENERATED_BODY()

public:

	OBSERVERFRAMEWORK_API FActorTrigger();

	void Clear()
	{
		ActorTag = "";
	}

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName ActorTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EBehaviorType BehaviorTo;
};

USTRUCT(BlueprintType)
struct FActorDistanceTrigger
{
	GENERATED_BODY()

public:

	OBSERVERFRAMEWORK_API FActorDistanceTrigger();

	void Clear()
	{
		ActorTag = "";
		Distance = 0.f;
		LineOfSight = false;
	}

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName ActorTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EBehaviorType BehaviorTo;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Distance;

	UPROPERTY(BlueprintReadWrite)
	bool LineOfSight;
};

USTRUCT(BlueprintType)
struct FBehaviorIdleArt
{
	GENERATED_BODY()

public:

	OBSERVERFRAMEWORK_API FBehaviorIdleArt();

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EIdleArt IdleArt = EIdleArt::Chilled;
};

USTRUCT(BlueprintType)
struct FBehaviorFollowArt
{
	GENERATED_BODY()

public:

	OBSERVERFRAMEWORK_API FBehaviorFollowArt();

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EFollowArt FollowArt = EFollowArt::Standard;
};

USTRUCT(BlueprintType)
struct FBehaviorPatrolArt
{
	GENERATED_BODY()

public:

	OBSERVERFRAMEWORK_API FBehaviorPatrolArt();

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EPatrolArt PatrolArt = EPatrolArt::Spline;
};

USTRUCT(BlueprintType)
struct FBehaviorBasic
{
	GENERATED_BODY()

public:

	OBSERVERFRAMEWORK_API FBehaviorBasic();

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EBehaviorType BehaviorType;

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MovementSpeed;

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FActorTrigger> OnSightTriggers;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FActorTrigger> LoseSightTriggers;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FActorDistanceTrigger> WithinDistanceTriggers;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FActorDistanceTrigger> BeyondDistanceTriggers;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FName> TargetTags;

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "BehaviorType == EBehaviorType::Idle"))
	FBehaviorIdleArt BehaviorIdleArt;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "BehaviorType == EBehaviorType::Follow"))
	FBehaviorFollowArt BehaviorFollowArt;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "BehaviorType == EBehaviorType::Patrol"))
	FBehaviorPatrolArt BehaviorPatrolArt;
};

USTRUCT(BlueprintType)
struct FPerceptionReact
{
	GENERATED_BODY()

public:

	OBSERVERFRAMEWORK_API FPerceptionReact();

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	UAnimMontage* OnPerceivedMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	UAnimMontage* OnLostMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	UAnimMontage* OnForgottenMontage;

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	USoundBase* OnPerceivedSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	USoundBase* OnLostSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	USoundBase* OnForgottenSound;
};
