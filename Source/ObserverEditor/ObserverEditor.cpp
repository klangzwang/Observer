#include "ObserverEditor.h"
#include "GameEditorToolSet.h"
#include "GameEditorSnippets.h"
#include "SlateExtras.h"

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

		FGlobalTabmanager::Get()->RegisterNomadTabSpawner("Snippets",
			FOnSpawnTab::CreateRaw(this, &FGameEditorSnippets::OnSpawnTab))
			.SetDisplayName(FText::FromString("Snippets"))
			.SetMenuType(ETabSpawnerMenuType::Enabled);
	}

	virtual void ShutdownModule() override
	{
		if (UObjectInitialized())
		{
			if (ToolMenusHandle.IsValid())
			{
				UToolMenus::UnRegisterStartupCallback(ToolMenusHandle);
			}
		}

		FGlobalTabmanager::Get()->UnregisterNomadTabSpawner("Snippets");
	}

private:

	FDelegateHandle ToolMenusHandle;
};

IMPLEMENT_MODULE(FObserverEditorModule, ObserverEditor);

#undef LOCTEXT_NAMESPACE