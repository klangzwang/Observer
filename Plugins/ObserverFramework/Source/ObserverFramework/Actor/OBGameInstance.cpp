#include "OBGameInstance.h"
#include "OBLocalPlayer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBGameInstance)

UOBGameInstance::UOBGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

int32 UOBGameInstance::AddLocalPlayer(ULocalPlayer* NewPlayer, FPlatformUserId UserId)
{
	int32 ReturnVal = Super::AddLocalPlayer(NewPlayer, UserId);
	if (ReturnVal != INDEX_NONE)
	{
		if (!PrimaryPlayer.IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("AddLocalPlayer: Set %s to Primary Player"), *NewPlayer->GetName());
			PrimaryPlayer = Cast<UOBLocalPlayer>(NewPlayer);
		}

		K2_OnAddLocalPlayer(PrimaryPlayer.Get(), UserId);
	}

	return ReturnVal;
}

bool UOBGameInstance::RemoveLocalPlayer(ULocalPlayer* ExistingPlayer)
{
	if (PrimaryPlayer == Cast<UOBLocalPlayer>(ExistingPlayer))
	{
		PrimaryPlayer.Reset();
		UE_LOG(LogTemp, Log, TEXT("RemoveLocalPlayer: Unsetting Primary Player from %s"), *ExistingPlayer->GetName());
	}

	K2_OnRemoveLocalPlayer(PrimaryPlayer.Get());

	return Super::RemoveLocalPlayer(ExistingPlayer);
}