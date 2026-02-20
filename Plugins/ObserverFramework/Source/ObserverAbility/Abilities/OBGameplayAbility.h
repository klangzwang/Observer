#pragma once
#include "Abilities/GameplayAbility.h"
#include "OBGameplayAbility.generated.h"

class UOBAbilitySystemComponent;

/**
 * UOBGameplayAbility
 *
 *	The base gameplay ability class used by this project.
 */
UCLASS(MinimalAPI, Abstract, HideCategories = Input, Meta = (ShortTooltip = "The base gameplay ability class used by this project."))
class UOBGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	friend class UOBAbilitySystemComponent;

public:

	UOBGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};