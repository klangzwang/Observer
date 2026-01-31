#include "OBTypes.h"

FActorTrigger::FActorTrigger()
{
	ActorTag = FName();
	BehaviorTo = EBehaviorType::Idle;
}

FActorDistanceTrigger::FActorDistanceTrigger()
{
	ActorTag = FName();
	Distance = 0.f;
	BehaviorTo = EBehaviorType::Idle;
	LineOfSight = false;
}

FBehaviorBasic::FBehaviorBasic()
{
	BehaviorType = EBehaviorType::Idle;
	MovementSpeed = 0.f;
	TargetTags.Init("", 0);
	TargetTags.Add("Player");
	TargetTags.Add("PatrolPoint");
}

FBehaviorIdleArt::FBehaviorIdleArt()
{
}

FBehaviorFollowArt::FBehaviorFollowArt()
{
}

FBehaviorPatrolArt::FBehaviorPatrolArt()
{
}

FPerceptionReact::FPerceptionReact()
{
	OnPerceivedMontage = nullptr;
	OnLostMontage = nullptr;
	OnForgottenMontage = nullptr;
	OnPerceivedSound = nullptr;
	OnLostSound = nullptr;
	OnForgottenSound = nullptr;
}

FOBHUDLayoutRequest::FOBHUDLayoutRequest()
{
	LayoutClass = nullptr;
	LayerID = FGameplayTag();
}
