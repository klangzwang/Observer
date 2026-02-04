#include "ObserverEditor.h"
#include "DataValidationModule.h"
#include "Editor/UnrealEdEngine.h"
#include "Engine/GameInstance.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "GameEditorStyle.h"
#include "ToolMenu.h"
#include "ToolMenus.h"
#include "UObject/UObjectIterator.h"
#include "UnrealEdGlobals.h"
#include "IPythonScriptPlugin.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "EditorUtilitySubsystem.h"

class SWidget;

#define LOCTEXT_NAMESPACE "ObserverEditor"

DEFINE_LOG_CATEGORY(LogObserverEditor);

static bool HasPlayWorld()
{
	return GEditor->PlayWorld != nullptr;
}

static bool HasNoPlayWorld()
{
	return !HasPlayWorld();
}

static bool HasPlayWorldAndRunning()
{
	return HasPlayWorld() && !GUnrealEd->PlayWorld->bDebugPauseExecution;
}

static void OpenValeaTools_Clicked()
{
	IPythonScriptPlugin* PythonPlugin = IPythonScriptPlugin::Get();

	if (!PythonPlugin)
		return;

	FString ScriptPath = "";
	FString ExecCommand = "";

	//
	// Bridge
	//
	FString ProjectInfo = FString::Printf(TEXT("observer_context = {'project_name': 'Observer', 'developer': 'Valea', 'engine_version': '5.6'}"));
	PythonPlugin->ExecPythonCommand(*ProjectInfo);

	ScriptPath = FPaths::ProjectContentDir() / TEXT("Editor/Python/ValeaTools_Bridge.py");
	ExecCommand = FString::Printf(TEXT("if os.path.exists('%s'): exec(open('%s').read())"), *ScriptPath, *ScriptPath);

	PythonPlugin->ExecPythonCommand(TEXT("import os"));
	PythonPlugin->ExecPythonCommand(*ExecCommand);

	//
	// PSD Importer
	//
	ScriptPath = FPaths::ProjectContentDir() / TEXT("Editor/Python/PSDImporter.py");
	ExecCommand = FString::Printf(TEXT("import PSDImporter; PSDImporter.run_psd_import()"), *ScriptPath);
	PythonPlugin->ExecPythonCommand(*ExecCommand);

	//
	// NoBridge
	//
	ExecCommand = TEXT("print('Hallo Valea! Der Observer beobachtet dich...') ");
	PythonPlugin->ExecPythonCommand(*ExecCommand);
	//
	ScriptPath = FPaths::ProjectContentDir() / TEXT("Editor/Python/ValeaTools_Init.py");
	ExecCommand = FString::Printf(TEXT("exec(open('%s').read())"), *ScriptPath);
	PythonPlugin->ExecPythonCommand(*ExecCommand);

	//
	// EditorUtilityWidget
	//
	FSoftObjectPath WidgetAssetPath(TEXT("/Game/Editor/UtilityWidget/EUW_ValeaTools.EUW_ValeaTools"));
	UObject* WidgetAsset = WidgetAssetPath.TryLoad();

	if (UEditorUtilityWidgetBlueprint* WidgetBlueprint = Cast<UEditorUtilityWidgetBlueprint>(WidgetAsset))
	{
		UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
		if (EditorUtilitySubsystem)
		{
			EditorUtilitySubsystem->SpawnAndRegisterTab(WidgetBlueprint);
		}
	}

	//
	// Batch
	// 
	ScriptPath = FPaths::ProjectContentDir() / TEXT("Editor/Batch/Test.bat");
	FString Arguments = FString::Printf(TEXT("\"%s\""), *FPaths::GetProjectFilePath());

	// Den Prozess starten
	// bLaunchDetached = true (läuft im Hintergrund weiter)
	// bLaunchHidden = false (zeigt das CMD-Fenster an, nützlich für Debugging)
	FProcHandle ProcessHandle = FPlatformProcess::CreateProc(
		*ScriptPath,
		*Arguments,
		true, false, false,
		nullptr, 0, nullptr, nullptr
	);

	if (ProcessHandle.IsValid())
	{
		UE_LOG(LogObserverEditor, Log, TEXT("ValeaTools: Batch-Skript erfolgreich gestartet."));
		FPlatformProcess::CloseProc(ProcessHandle);
	}
	else
	{
		UE_LOG(LogObserverEditor, Error, TEXT("ValeaTools: Batch-Skript konnte nicht gefunden werden unter: %s"), *ScriptPath);
	}
}

static bool CanShowValeaTools()
{
	return HasNoPlayWorld();
}

static TSharedRef<SWidget> GetValeaToolsDropdown()
{
	FMenuBuilder MenuBuilder(true, nullptr);
	
	const FText DisplayName = FText::FromString("ValeaTools");
	MenuBuilder.AddMenuEntry(
		DisplayName,
		LOCTEXT("ValeaToolsDescription", "Opens Valea Tools in the editor"),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateStatic(&OpenValeaTools_Clicked),
			FCanExecuteAction::CreateStatic(&HasNoPlayWorld),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateStatic(&HasNoPlayWorld)
		)
	);

	return MenuBuilder.MakeWidget();
}

static void RegisterGameEditorMenus()
{
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
	FToolMenuSection& Section = Menu->AddSection("PlayGameExtensions", TAttribute<FText>(), FToolMenuInsert("Play", EToolMenuInsertType::After));

	FToolMenuEntry ValeaToolsEntry = FToolMenuEntry::InitComboButton(
		"ValeaToolsOptions",
		FUIAction(
			FExecuteAction(),
			FCanExecuteAction::CreateStatic(&HasNoPlayWorld),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateStatic(&CanShowValeaTools)),
		FOnGetContent::CreateStatic(&GetValeaToolsDropdown),
		LOCTEXT("ValeaTools_Label", "Valea Tools"),
		LOCTEXT("ValeaTools_ToolTip", "Some Tools while using the editor"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Level")
		//FSlateIcon(FGameEditorStyle::GetStyleSetName(), "GameEditor.CheckContent")
	);

	ValeaToolsEntry.StyleNameOverride = "CalloutToolbar";

	Section.AddEntry(ValeaToolsEntry);
}

class FObserverEditorModule : public FDefaultGameModuleImpl
{
	typedef FObserverEditorModule ThisClass;

	virtual void StartupModule() override
	{
		FGameEditorStyle::Initialize();

		if (!IsRunningGame())
		{
			//FModuleManager::Get().OnModulesChanged().AddRaw(this, &FObserverEditorModule::ModulesChangedCallback);

			if (FSlateApplication::IsInitialized())
			{
				ToolMenusHandle = UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateStatic(&RegisterGameEditorMenus));
			}

			//FEditorDelegates::BeginPIE.AddRaw(this, &ThisClass::OnBeginPIE);
			//FEditorDelegates::EndPIE.AddRaw(this, &ThisClass::OnEndPIE);
		}
	}

	/*
	void OnBeginPIE(bool bIsSimulating)
	{
	}

	void OnEndPIE(bool bIsSimulating)
	{
	}
	*/

	virtual void ShutdownModule() override
	{
		//FEditorDelegates::BeginPIE.RemoveAll(this);
		//FEditorDelegates::EndPIE.RemoveAll(this);

		if (UObjectInitialized() && ToolMenusHandle.IsValid())
		{
			UToolMenus::UnRegisterStartupCallback(ToolMenusHandle);
		}

		//FModuleManager::Get().OnModulesChanged().RemoveAll(this);
		FGameEditorStyle::Shutdown();
	}

/*
protected:

	void ModulesChangedCallback(FName ModuleThatChanged, EModuleChangeReason ReasonForChange)
	{
		if ((ReasonForChange == EModuleChangeReason::ModuleLoaded))
		{
		}
	}
*/

private:

	FDelegateHandle ToolMenusHandle;
};

IMPLEMENT_MODULE(FObserverEditorModule, ObserverEditor);

#undef LOCTEXT_NAMESPACE
