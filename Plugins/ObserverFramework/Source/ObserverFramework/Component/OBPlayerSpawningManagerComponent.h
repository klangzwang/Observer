#pragma once
#include "Components/GameStateComponent.h"
#include "OBPlayerSpawningManagerComponent.generated.h"

class AController;
class APlayerController;
class APlayerState;
class APlayerStart;
class AOBPlayerStart;
class AActor;

/**
 * @class UOBPlayerSpawningManagerComponent
 */
UCLASS(MinimalAPI)
class UOBPlayerSpawningManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:

	UOBPlayerSpawningManagerComponent(const FObjectInitializer& ObjectInitializer);

	/** UActorComponent */
	virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	/** ~UActorComponent */

protected:

	// Utility
	APlayerStart* GetFirstRandomUnoccupiedPlayerStart(AController* Controller, const TArray<AOBPlayerStart*>& FoundStartPoints) const;
	
	virtual AActor* OnChoosePlayerStart(AController* Player, TArray<AOBPlayerStart*>& PlayerStarts) { return nullptr; }
	virtual void OnFinishRestartPlayer(AController* Player, const FRotator& StartRotation) { }

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName=OnFinishRestartPlayer))
	void K2_OnFinishRestartPlayer(AController* Player, const FRotator& StartRotation);

private:

	/** We proxy these calls from ALyraGameMode, to this component so that each experience can more easily customize the respawn system they want. */
	AActor* ChoosePlayerStart(AController* Player);
	bool ControllerCanRestart(AController* Player);
	void FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation);
	friend class AOBGameMode;
	/** ~ALyraGameMode */

	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<AOBPlayerStart>> CachedPlayerStarts;

private:

	void OnLevelAdded(ULevel* InLevel, UWorld* InWorld);
	void HandleOnActorSpawned(AActor* SpawnedActor);

#if WITH_EDITOR
	APlayerStart* FindPlayFromHereStart(AController* Player);
#endif
};

#undef UE_API
