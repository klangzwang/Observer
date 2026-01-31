#pragma once
#include "NativeGameplayTags.h"

namespace ObserverTags
{
	OBSERVERFRAMEWORK_API FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Crouch);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Aim);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Use);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Attack);
}
