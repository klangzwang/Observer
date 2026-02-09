#pragma once
#include "GameFramework/PlayerState.h"
#include "GenericTeamAgentInterface.h"
#include "OBPlayerState.generated.h"

class AController;
class AOBPlayerController;

inline int32 GenericTeamIdToInteger(FGenericTeamId ID)
{
	return (ID == FGenericTeamId::NoTeam) ? INDEX_NONE : (int32)ID;
}

inline FGenericTeamId IntegerToGenericTeamId(int32 ID)
{
	return (ID == INDEX_NONE) ? FGenericTeamId::NoTeam : FGenericTeamId((uint8)ID);
}

UCLASS(MinimalAPI, Blueprintable)
class AOBPlayerState : public APlayerState, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:

	AOBPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Observer|PlayerState")
	AOBPlayerController* GetOBPlayerController() const;

	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;

	/** Returns the Team ID of the team the player belongs to. */
	UFUNCTION(BlueprintCallable)
	int32 GetTeamId() const
	{
		return GenericTeamIdToInteger(MyTeamID);
	}

private:

	UPROPERTY()
	FGenericTeamId MyTeamID;
};
