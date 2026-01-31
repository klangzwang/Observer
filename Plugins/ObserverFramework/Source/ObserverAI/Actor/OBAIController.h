#pragma once
#include "CoreMinimal.h"
#include "DetourCrowdAIController.h"
#include "OBTypes.h"
#include "OBAIController.generated.h"

class AOBAICharacter;
class UOBAIComponent;
class UAIPerceptionTypes;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSightProgressingDelegate, float, Progress);

UCLASS(abstract)
class AOBAIController : public ADetourCrowdAIController
{
	GENERATED_BODY()

public:

    AOBAIController();

	virtual void Tick(float DeltaTime) override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

protected:

	FRotator SmoothTargetRotation;
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Observer|AIController")
	float InterpToControlRotSpeed = 30.0f;

public:

	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn) override;

protected:

	UFUNCTION()
	void OnTargetPerceived(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	void OnTargetForgotten(AActor* Actor);

protected:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Observer|AIController")
	UAISenseConfig_Sight* SightConfig;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Observer|AIController")
	UAISenseConfig_Hearing* HearConfig;

	UPROPERTY(BlueprintReadOnly, Category = "Observer|AIController")
	TObjectPtr<class AOBAICharacter> CharacterOwned;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Observer|AIController")
	TObjectPtr<class UOBAIComponent> AIComponent;

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Observer|AIController")
	bool PartialPath;

protected:

	FTimerHandle DetectionTimerHandle;

	UFUNCTION()
	void TickPerceived();

	UFUNCTION()
	void TickLost();

	//UFUNCTION()
	//void RootBehavior();

private:

	UPROPERTY(EditAnywhere, Category = "Observer|AIController")
	float CurrentDetectionLevel = 0.f;

	UPROPERTY(EditAnywhere, Category = "Observer|AIController")
	float DetectionDelay = 0.f;

public:

	UPROPERTY(BlueprintAssignable, Category = "Observer|AIController")
	FSightProgressingDelegate OnTargetPerceivedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Observer|AIController")
	FSightProgressingDelegate OnTargetLostDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Observer|AIController")
	FSightProgressingDelegate OnTargetForgottenDelegate;

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Observer|AIController")
	FPerceptionReact PerceptionReaction;
};
