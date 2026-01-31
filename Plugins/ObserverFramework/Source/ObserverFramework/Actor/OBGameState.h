#pragma once
#include "GameFramework/GameState.h"
#include "OBGameState.generated.h"

UCLASS(MinimalAPI, Blueprintable)
class AOBGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:

	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};

UCLASS(MinimalAPI, Blueprintable)
class AOBGameState : public AGameState
{
	GENERATED_BODY()
	
public:

    AOBGameState();
	
public:

	UFUNCTION(BlueprintPure, Category = "Observer|GameState")
	static AOBGameState* FindGameState(const UWorld* World) { return (World ? Cast<AOBGameState>(World->GetGameState()) : nullptr); }

	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
