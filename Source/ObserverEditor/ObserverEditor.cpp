#include "ObserverEditor.h"
#include "GameEditorToolSet.h"

#define LOCTEXT_NAMESPACE "ObserverEditor"

DEFINE_LOG_CATEGORY(LogObserverEditor);

class FObserverEditorModule : public FDefaultGameModuleImpl
{
	typedef FObserverEditorModule ThisClass;

	virtual void StartupModule() override
	{
		if (!IsRunningGame())
		{
			if (FSlateApplication::IsInitialized())
			{
				ToolMenusHandle = UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateStatic(&FGameEditorToolSet::RegisterGameEditorMenus));
			}
		}
	}

	virtual void ShutdownModule() override
	{
		if (UObjectInitialized() && ToolMenusHandle.IsValid())
		{
			UToolMenus::UnRegisterStartupCallback(ToolMenusHandle);
		}
	}

private:

	FDelegateHandle ToolMenusHandle;
};

IMPLEMENT_MODULE(FObserverEditorModule, ObserverEditor);

#undef LOCTEXT_NAMESPACE
