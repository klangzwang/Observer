#pragma once
#include "AbilitySystemComponent.h"
#include "OBAbilitySystemComponent.generated.h"

/**
 * UOBAbilitySystemComponent
 *
 *	Base ability system component class used by this project.
 */
UCLASS(MinimalAPI, Blueprintable, Meta = (BlueprintSpawnableComponent))
class UOBAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:

	UOBAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
