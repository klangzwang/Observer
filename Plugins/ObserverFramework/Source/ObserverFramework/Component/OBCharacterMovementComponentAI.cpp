#include "OBCharacterMovementComponentAI.h"

UOBCharacterMovementComponentAI::UOBCharacterMovementComponentAI(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    //bUseRVOAvoidance = true;
}

void UOBCharacterMovementComponentAI::BeginPlay()
{
	Super::BeginPlay();
}

void UOBCharacterMovementComponentAI::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
