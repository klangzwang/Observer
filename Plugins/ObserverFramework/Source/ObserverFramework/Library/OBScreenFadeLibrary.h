#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OBScreenFadeLibrary.generated.h"

UCLASS()
class OBSERVERFRAMEWORK_API UOBScreenFadeLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "Observer|Fade", meta = (WorldContext = "WorldContextObject"))
	static void FadeDelegate(UObject* WorldContextObject, bool bFadeToBlack);
};
