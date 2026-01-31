#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "OBGameSession.generated.h"

UCLASS(Blueprintable)
class AOBGameSession : public AGameSession {
	
    GENERATED_BODY()
	
public:

    AOBGameSession(const FObjectInitializer& ObjectInitializer);
};
