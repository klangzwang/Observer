#pragma once
#include "GameFramework/PlayerController.h"
#include "OBPlayerController.generated.h"

UCLASS(Blueprintable, config = Game)
class OBSERVERFRAMEWORK_API AOBPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	AOBPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PreInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void ReceivedPlayer() override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetPawn(APawn* InPawn) override;
	virtual void OnPossess(class APawn* APawn) override;
	virtual void OnUnPossess() override;

protected:

	virtual void OnRep_PlayerState() override;
};
