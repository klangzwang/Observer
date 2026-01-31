#include "OBInputComponent.h"
#include "EnhancedInputSubsystems.h"

UOBInputComponent::UOBInputComponent(const FObjectInitializer& ObjectInitializer)
{
}

void UOBInputComponent::AddInputMappings(const UOBInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);
}

void UOBInputComponent::RemoveInputMappings(const UOBInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);
}

void UOBInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}