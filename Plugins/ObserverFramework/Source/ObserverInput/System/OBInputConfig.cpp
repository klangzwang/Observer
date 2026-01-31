#include "OBInputConfig.h"

//#include UE_INLINE_GENERATED_CPP_BY_NAME(OBInputConfig)

UOBInputConfig::UOBInputConfig(const FObjectInitializer& ObjectInitializer)
{
}

const UInputAction* UOBInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag) const
{
	for (const FOBInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}
	return nullptr;
}
