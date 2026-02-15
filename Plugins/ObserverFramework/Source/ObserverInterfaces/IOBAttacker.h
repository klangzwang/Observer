#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IOBAttacker.generated.h"

UINTERFACE(MinimalAPI, NotBlueprintable)
class UOBAttacker : public UInterface
{
	GENERATED_BODY()
};

class IOBAttacker
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Observer|Attacker")
	virtual void DoAttackTrace(FName DamageSourceBone) = 0;
};
