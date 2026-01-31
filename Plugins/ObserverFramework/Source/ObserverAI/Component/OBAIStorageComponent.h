#pragma once
#include "Components/GameStateComponent.h"
#include "OBAIStorageComponent.generated.h"

class AActor;

UCLASS(Abstract)
class UOBAIStorageComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:

	UOBAIStorageComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	UFUNCTION(BlueprintPure, Category = "Observer|AIStorage")
	static UOBAIStorageComponent* FindAIStorageComponent(const AGameStateBase* GameState) { return (GameState ? GameState->FindComponentByClass<UOBAIStorageComponent>() : nullptr); }

public:

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Observer|AIStorage")
	TArray<AActor*> AIOwners;
};
