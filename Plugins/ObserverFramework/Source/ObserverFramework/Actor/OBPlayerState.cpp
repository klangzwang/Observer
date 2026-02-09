#include "OBPlayerState.h"
#include "OBPlayerController.h"

AOBPlayerState::AOBPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MyTeamID = FGenericTeamId::NoTeam;
}

AOBPlayerController* AOBPlayerState::GetOBPlayerController() const
{
	return Cast<AOBPlayerController>(GetOwner());
}

void AOBPlayerState::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if(MyTeamID != NewTeamID)
		MyTeamID = NewTeamID;
}

FGenericTeamId AOBPlayerState::GetGenericTeamId() const
{
	return MyTeamID;
}
