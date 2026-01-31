#pragma once
#include "CoreMinimal.h"
#include "OBCharacter.h"
#include "OBPlayerController.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "OBAnimNotifyState.generated.h"

UCLASS()
class OBSERVERFRAMEWORK_API UOBAnimNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

public:

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	UFUNCTION(BlueprintCallable, Category = "Observer|AnimNotify")
	virtual void PerformNotifyBegin(AOBCharacter* Character, AOBPlayerController* Controller, USkeletalMeshComponent* skeletalMesh, UAnimSequenceBase* Animation);

	UFUNCTION(BlueprintCallable, Category = "Observer|AnimNotify")
	virtual void PerformNotifyTick(AOBCharacter* Character, AOBPlayerController* Controller, USkeletalMeshComponent* skeletalMesh, UAnimSequenceBase* Animation);

	UFUNCTION(BlueprintCallable, Category = "Observer|AnimNotify")
	virtual void PerformNotifyEnd(AOBCharacter* Character, AOBPlayerController* Controller, USkeletalMeshComponent* skeletalMesh, UAnimSequenceBase* Animation);
};