#include "GameEditorToolSet.h"
#include "Editor/UnrealEdEngine.h"
#include "UnrealEdGlobals.h"
#include "IPythonScriptPlugin.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "EditorUtilitySubsystem.h"

class SWidget;

#define LOCTEXT_NAMESPACE "GameEditorToolSet"

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

static bool CanShowValeaTools()
{
	return HasNoPlayWorld();
}

//
// ToolSetAction
//
static void OpenBlueprintTemplate()
{
	FSoftObjectPath WidgetAssetPath(TEXT("/Game/Editor/UtilityWidget/EUW_BlueprintTemplateWidget.EUW_BlueprintTemplateWidget"));
	UObject* WidgetAsset = WidgetAssetPath.TryLoad();

	if (UEditorUtilityWidgetBlueprint* WidgetBlueprint = Cast<UEditorUtilityWidgetBlueprint>(WidgetAsset))
	{
		UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
		if (EditorUtilitySubsystem)
		{
			EditorUtilitySubsystem->SpawnAndRegisterTab(WidgetBlueprint);
		}
	}
}

static void OpenPSDImporter()
{
	IPythonScriptPlugin* PythonPlugin = IPythonScriptPlugin::Get();

	if (!PythonPlugin)
		return;

	FString ScriptPath = FPaths::ProjectContentDir() / TEXT("Editor/Python/PSDImporter.py");
	FString ExecCommand = FString::Printf(TEXT("import PSDImporter; PSDImporter.run_psd_import()"), *ScriptPath);
	PythonPlugin->ExecPythonCommand(*ExecCommand);
}

//
// RegisterMenu
//
static TSharedRef<SWidget> GetValeaToolsDropdown()
{
	FMenuBuilder MenuBuilder(true, nullptr);

	//
	// BlueprintTemplates
	//
	const FText BlueprintTemplatesDisplayName = FText::FromString("BlueprintTemplates");
	MenuBuilder.AddMenuEntry(
		BlueprintTemplatesDisplayName,
		LOCTEXT("BlueprintTemplatesDescription", "Add/Save/Load Blueprint Templates"),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateStatic(&OpenBlueprintTemplate),
			FCanExecuteAction::CreateStatic(&HasNoPlayWorld),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateStatic(&HasNoPlayWorld)
		)
	);

	//
	// PSDImporter
	//
	const FText PSDImporterDisplayName = FText::FromString("PSDImporter");
	MenuBuilder.AddMenuEntry(
		PSDImporterDisplayName,
		LOCTEXT("PSDImporterDescription", "Import, then Crop all the Layers to Textures"),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateStatic(&OpenPSDImporter),
			FCanExecuteAction::CreateStatic(&HasNoPlayWorld),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateStatic(&HasNoPlayWorld)
		)
	);

	return MenuBuilder.MakeWidget();
}

void FGameEditorToolSet::RegisterGameEditorMenus()
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

#undef LOCTEXT_NAMESPACE

/*
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
		UE_LOG(LogTemp, Log, TEXT("ValeaTools: Batch-Skript erfolgreich gestartet."));
		FPlatformProcess::CloseProc(ProcessHandle);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ValeaTools: Batch-Skript konnte nicht gefunden werden unter: %s"), *ScriptPath);
	}
}
*/