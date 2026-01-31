#pragma once
#include "CoreMinimal.h"
#include "CommonGameViewportClient.h"
#include "OBGameViewportClient.generated.h"

class UGameInstance;
class UObject;

UCLASS()
class UOBGameViewportClient : public UCommonGameViewportClient
{
	GENERATED_BODY()

public:

	UOBGameViewportClient();

	virtual void Init(struct FWorldContext& WorldContext, UGameInstance* OwningGameInstance, bool bCreateNewAudioDevice = true) override;
};
