#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IOBLoadingProcess.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UOBLoadingProcess : public UInterface
{
	GENERATED_BODY()
};

class IOBLoadingProcess
{
	GENERATED_BODY()

public:

	static bool ShouldShowLoadingScreen(UObject* TestObject, FString& OutReason);

	virtual bool ShouldShowLoadingScreen(FString& OutReason) const
	{
		return false;
	}
};
