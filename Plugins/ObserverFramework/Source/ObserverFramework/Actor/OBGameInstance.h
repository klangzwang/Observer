#pragma once
#include "Engine/GameInstance.h"
#include "OBGameInstance.generated.h"

class UOBLocalPlayer;

UCLASS(Abstract, Config = Game)
class OBSERVERFRAMEWORK_API UOBGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UOBGameInstance(const FObjectInitializer& ObjectInitializer);

	virtual int32 AddLocalPlayer(ULocalPlayer* NewPlayer, FPlatformUserId UserId) override;
	virtual bool RemoveLocalPlayer(ULocalPlayer* ExistingPlayer) override;

public:

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnAddLocalPlayer", ScriptName = "OnAddLocalPlayer"))
	void K2_OnAddLocalPlayer(UOBLocalPlayer* NewPlayer, FPlatformUserId UserId);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnRemoveLocalPlayer", ScriptName = "OnRemoveLocalPlayer"))
	void K2_OnRemoveLocalPlayer(UOBLocalPlayer* ExistingPlayer);

private:

	TWeakObjectPtr<UOBLocalPlayer> PrimaryPlayer;
};
