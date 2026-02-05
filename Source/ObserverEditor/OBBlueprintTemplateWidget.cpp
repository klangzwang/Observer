#include "OBBlueprintTemplateWidget.h"
#include "EditorUtilityLibrary.h"
//#include "Templates/SharedPointer.h"
#include "IPythonScriptPlugin.h"
#include "Misc/Paths.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Commands/GenericCommands.h"
#include "Framework/Commands/UICommandList.h"

bool UOBBlueprintTemplateWidget::SaveBlueprintClipboardToTemplate(FString TemplateName)
{
	if (TemplateName.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Template Name ist leer!"));
		return false;
	}

	FString ClipboardContent;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);

	if (!IsValidBlueprintClipboardContent(ClipboardContent))
	{
		UE_LOG(LogTemp, Warning, TEXT("Clipboard enthält keine gültigen Blueprint Nodes!"));
		return false;
	}

	// 1. Python Teil: Auf die Festplatte schreiben
	FString SavePath = FPaths::ProjectSavedDir() / TEXT("Templates") / TemplateName + TEXT(".txt");

	FString PythonScript = FString::Printf(TEXT(
		"import unreal\n"
		"import os\n"
		"file_path = '%s'\n"
		"content = u\"\"\"%s\"\"\"\n" // Nutzen von Raw Strings im Python
		"os.makedirs(os.path.dirname(file_path), exist_ok=True)\n"
		"with open(file_path, 'w', encoding='utf-8') as f:\n"
		"    f.write(content)\n"
		"unreal.log('Blueprint Template gespeichert: ' + file_path)"
	), *SavePath, *ClipboardContent.Replace(TEXT("\""), TEXT("\\\""))); // Anführungszeichen escapen

	IPythonScriptPlugin::Get()->ExecPythonCommand(*PythonScript);

	// 2. C++ Teil: In die TMap laden
	TemplateStorage.Add(TemplateName, ClipboardContent);

	UE_LOG(LogTemp, Log, TEXT("Template '%s' erfolgreich gespeichert."), *TemplateName);
	return true;
}

FString UOBBlueprintTemplateWidget::LoadClipboardFromTemplate(FString TemplateName)
{
	if (!TemplateStorage.Contains(TemplateName))
	{
		UE_LOG(LogTemp, Warning, TEXT("Template '%s' nicht gefunden!"), *TemplateName);
		return FString();
	}

	FString Content = TemplateStorage[TemplateName];

	// 1. Inhalt in die Zwischenablage kopieren
	FPlatformApplicationMisc::ClipboardCopy(*Content);

	// 2. Befehl im aktuellen Kontext ausführen
	if (FSlateApplication::IsInitialized())
	{
		// Wir holen die CommandList des aktuell fokussierten Widgets.
		// Wenn der Blueprint-Editor offen ist, leitet er den Paste-Befehl korrekt weiter.
		TSharedRef<FUICommandList> ActionList = MakeShared<FUICommandList>();

		if (ActionList.IsUnique())
		{
			// Führt den 'Paste' Befehl auf der aktiven CommandList aus
			ActionList->ExecuteAction(FGenericCommands::Get().Paste.ToSharedRef());
			UE_LOG(LogTemp, Log, TEXT("Template '%s' eingefügt."), *TemplateName);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Kein aktives Fenster mit CommandList gefunden. Paste fehlgeschlagen."));
		}
	}

	return Content;
}

bool UOBBlueprintTemplateWidget::IsValidBlueprintClipboardContent(const FString& ClipboardContent)
{
	return ClipboardContent.StartsWith(TEXT("Begin Object Class="));
}