#pragma once
#include "EnhancedInputComponent.h"
#include "OBInputConfig.h"
#include "OBInputComponent.generated.h"

class UEnhancedInputLocalPlayerSubsystem;
class UObject;

UCLASS(Config = Input)
class OBSERVERINPUT_API UOBInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:

	UOBInputComponent(const FObjectInitializer& ObjectInitializer);

	template<class UserClass, typename FuncType>
	void BindNativeAction(const UOBInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound);

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UOBInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);

	template<class UserClass, typename FuncType>
	void BindNativeActionByTag(UEnhancedInputComponent* EnhancedInputComponent, UOBInputConfig* InputSettings, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func);

public:

	void AddInputMappings(const UOBInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
	void RemoveInputMappings(const UOBInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;

	void RemoveBinds(TArray<uint32>& BindHandles);

private:

	UPROPERTY(Transient, VisibleInstanceOnly)
	TSet<int32> InputEventBindingHandles;
};

template<class UserClass, typename FuncType>
void UOBInputComponent::BindNativeAction(const UOBInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound)
{
	check(InputConfig);
	if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(InputTag, bLogIfNotFound))
	{
		BindAction(IA, TriggerEvent, Object, Func);
	}
}

template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UOBInputComponent::BindAbilityActions(const UOBInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles)
{
	check(InputConfig);

	for (const FOBInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, Action.InputTag).GetHandle());
			}

			if (ReleasedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag).GetHandle());
			}
		}
	}
}

template<class UserClass, typename FuncType>
void UOBInputComponent::BindNativeActionByTag(UEnhancedInputComponent* EnhancedInputComponent, UOBInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func)
{
	check(InputConfig);
	if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(InputTag))
	{
		const FEnhancedInputActionEventBinding& Binding = EnhancedInputComponent->BindAction(IA, TriggerEvent, Object, Func);
		InputEventBindingHandles.Add(Binding.GetHandle());
	}
}