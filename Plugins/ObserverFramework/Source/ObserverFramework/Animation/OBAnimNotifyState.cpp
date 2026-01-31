#include "OBAnimNotifyState.h"
#include "Engine.h"

void UOBAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Received_NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp != NULL && MeshComp->GetOwner() != NULL)
	{
		auto Character = Cast<AOBCharacter>(MeshComp->GetOwner());
		if (Character != NULL)
		{
			auto Controller = Cast<AOBPlayerController>(Character->GetController());
			if (Character != NULL && Controller != NULL)
			{
				PerformNotifyTick(Character, Controller, MeshComp, Animation);
			}
		}
	}
}

void UOBAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Received_NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (MeshComp != NULL && MeshComp->GetOwner() != NULL)
	{
		auto Character = Cast<AOBCharacter>(MeshComp->GetOwner());
		if (Character != NULL)
		{
			auto Controller = Cast<AOBPlayerController>(Character->GetController());
			if (Character != NULL && Controller != NULL)
			{
				PerformNotifyTick(Character, Controller, MeshComp, Animation);
			}
		}
	}
}

void UOBAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Received_NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp != NULL && MeshComp->GetOwner() != NULL)
	{
		auto Character = Cast<AOBCharacter>(MeshComp->GetOwner());
		if (Character != NULL)
		{
			auto Controller = Cast<AOBPlayerController>(Character->GetController());
			if (Character != NULL && Controller != NULL)
			{
				PerformNotifyEnd(Character, Controller, MeshComp, Animation);
			}
		}
	}
}

void UOBAnimNotifyState::PerformNotifyBegin(AOBCharacter* Character, AOBPlayerController* Controller, USkeletalMeshComponent* skeletalMesh, UAnimSequenceBase* Animation)
{
}

void UOBAnimNotifyState::PerformNotifyTick(AOBCharacter* Character, AOBPlayerController* Controller, USkeletalMeshComponent* skeletalMesh, UAnimSequenceBase* Animation)
{
}

void UOBAnimNotifyState::PerformNotifyEnd(AOBCharacter* Character, AOBPlayerController* Controller, USkeletalMeshComponent* skeletalMesh, UAnimSequenceBase* Animation)
{
}