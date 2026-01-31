#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IOBOutlineable.generated.h"

UINTERFACE(MinimalAPI, NotBlueprintable)
class UOBOutlineable : public UInterface
{
	GENERATED_BODY()
};

class IOBOutlineable
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Observer|Outlines")
	virtual void ShowOutlines(bool bActive) = 0;
};
