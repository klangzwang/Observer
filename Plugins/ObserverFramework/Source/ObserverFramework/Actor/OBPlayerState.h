#pragma once
#include "GameFramework/PlayerState.h"
#include "OBPlayerState.generated.h"

UCLASS(MinimalAPI, Blueprintable)
class AOBPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	AOBPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
