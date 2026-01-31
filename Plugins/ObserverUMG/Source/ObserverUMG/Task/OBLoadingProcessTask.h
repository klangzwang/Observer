#pragma once
#include "IOBLoadingProcess.h"
#include "UObject/Object.h"
#include "OBLoadingProcessTask.generated.h"

struct FFrame;

UCLASS(MinimalAPI, BlueprintType)
class UOBLoadingProcessTask : public UObject, public IOBLoadingProcess
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, meta=(WorldContext = "WorldContextObject"))
	static UOBLoadingProcessTask* CreateLoadingScreenProcessTask(UObject* WorldContextObject, const FString& ShowLoadingScreenReason);

public:

	UOBLoadingProcessTask() { }

	UFUNCTION(BlueprintCallable)
	void Unregister();

	UFUNCTION(BlueprintCallable)
	void SetShowLoadingScreenReason(const FString& InReason);

	virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
	
	FString Reason;
};
