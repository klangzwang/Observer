#include "OBCharacterMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBCharacterMovementComponent)

UOBCharacterMovementComponent::UOBCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UOBCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UOBCharacterMovementComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
