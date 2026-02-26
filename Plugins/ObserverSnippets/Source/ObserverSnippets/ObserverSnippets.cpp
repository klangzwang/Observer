#include "ObserverSnippets.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "LevelEditor.h"
#include "Widgets/Input/SEditableTextBox.h"

#include "HAL/PlatformApplicationMisc.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonObject.h"

#include "BlueprintEditor.h"
#include "Subsystems/AssetEditorSubsystem.h" 
#include "EdGraph/EdGraphNode.h"

#include "EdGraphUtilities.h"

#include "Widgets/Colors/SColorBlock.h"

#include "GraphEditorDragDropAction.h"
#include "ScopedTransaction.h"
#include "Misc/FileHelper.h"

#define LOCTEXT_NAMESPACE "ObserverSnippets"

DEFINE_LOG_CATEGORY(LogObserverSnippets);

//
// Custom Drag-and-Drop Action for Snippets
// Erbt von FGraphEditorDragDropAction, um die Snippet-Daten zu transportieren und beim Drop zu verarbeiten
// Beim Drop werden die Snippet-Daten in Nodes umgewandelt und an der Mausposition im Graphen platziert
//
class FSnippetDragDropAction : public FGraphEditorDragDropAction
{

public:

    DRAG_DROP_OPERATOR_TYPE(FSnippetDragDropAction, FGraphEditorDragDropAction)

        FString SnippetData;

    static TSharedRef<FSnippetDragDropAction> New(const FString& InSnippetData)
    {
        TSharedRef<FSnippetDragDropAction> Operation = MakeShared<FSnippetDragDropAction>();
        Operation->SnippetData = InSnippetData;
        Operation->Construct();
        return Operation;
    }

    virtual FReply DroppedOnPanel(const TSharedRef<class SWidget>& Panel, FVector2D ScreenPosition, FVector2D GraphPosition, UEdGraph& Graph) override
    {
        if (SnippetData.IsEmpty()) return FReply::Unhandled();

        // Transaktion starten für Undo/Redo
        const FScopedTransaction Transaction(FText::FromString("Drop Snippet"));
        Graph.Modify();

        TSet<UEdGraphNode*> PastedNodes;
        FEdGraphUtilities::ImportNodesFromText(&Graph, SnippetData, /*out*/ PastedNodes);

        // Position berechnen, um die Nodes exakt an der Maus (GraphPosition) zu zentrieren
        if (PastedNodes.Num() > 0)
        {
            FVector2D MinNodePos(TNumericLimits<float>::Max(), TNumericLimits<float>::Max());

            // Finde den Node, der am weitesten links/oben ist
            for (UEdGraphNode* Node : PastedNodes)
            {
                MinNodePos.X = FMath::Min(MinNodePos.X, (float)Node->NodePosX);
                MinNodePos.Y = FMath::Min(MinNodePos.Y, (float)Node->NodePosY);
            }

            // Offset zur Mausposition berechnen
            FVector2D Offset = GraphPosition - MinNodePos;

            // Alle Nodes verschieben und ans Grid snappen
            for (UEdGraphNode* Node : PastedNodes)
            {
                Node->NodePosX += Offset.X;
                Node->NodePosY += Offset.Y;
                Node->SnapToGrid(16);
            }

            Graph.NotifyGraphChanged();
        }

        return FReply::Handled();
    }
};

//
// Hauptmodul der ObserverSnippets-Plugin
// Registriert einen neuen Tab "Snippets" im Editor, der eine Liste aller gespeicherten Snippets anzeigt
// Ermöglicht das Erstellen neuer Snippets aus markierten Nodes im EventGraph und das Speichern als JSON-Datei
//
class FObserverSnippetsModule : public FDefaultGameModuleImpl
{
	typedef FObserverSnippetsModule ThisClass;

	virtual void StartupModule() override
	{
        FGlobalTabmanager::Get()->RegisterNomadTabSpawner("Snippets",
        FOnSpawnTab::CreateRaw(this, &FObserverSnippetsModule::OnSpawnTab))
        .SetDisplayName(FText::FromString("Snippets"))
        .SetMenuType(ETabSpawnerMenuType::Enabled);

        RefreshSnippetList();
	}

	virtual void ShutdownModule() override
	{
		FGlobalTabmanager::Get()->UnregisterNomadTabSpawner("Snippets");
	}

private:

    TSharedRef<SDockTab> OnSpawnTab(const FSpawnTabArgs& SpawnTabArgs)
    {
        return SNew(SDockTab)
            .TabRole(ETabRole::NomadTab)
            [
                SNew(SVerticalBox)

                    + SVerticalBox::Slot().AutoHeight().Padding(0, 5)
                    [
                        SNew(SHorizontalBox)

                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            [
                                SNew(SBox)
                                    .WidthOverride(24.0f)
                                    .HeightOverride(128.0f)
                                    [
                                        SNew(SOverlay)
                                            + SOverlay::Slot()
                                            [
                                                SNew(SColorBlock).Color(FLinearColor::Black)
                                            ]
                                            + SOverlay::Slot()
                                            .HAlign(HAlign_Center)
                                            .VAlign(VAlign_Bottom)
                                            [
                                                SNew(STextBlock)
                                                .Text(FText::FromString("SNIPPETS"))
                                                .Font(FAppStyle::GetFontStyle("BoldFont"))
                                                .RenderTransform(FSlateRenderTransform(FQuat2D(FMath::DegreesToRadians(-90.0f))))
                                                .RenderTransformPivot(FVector2D(0.5f, 0.5f))
                                            ]
                                    ]
                            ]

                        + SHorizontalBox::Slot()
                            .FillWidth(1.0f)
                            .Padding(0.f)
                            [
                                SNew(SVerticalBox)

                                    + SVerticalBox::Slot()
                                    .FillHeight(1.0f)
                                    [
                                        SAssignNew(SnippetListView, SListView<TSharedPtr<FString>>)
                                            .ListItemsSource(&SnippetList)
                                            .OnGenerateRow_Raw(this, &FObserverSnippetsModule::GenerateSnippetRow)
                                    ]
                            ]
                    ]

                // --- Trennlinie ---
                + SVerticalBox::Slot().AutoHeight().Padding(0, 5)[SNew(SSeparator).Orientation(Orient_Horizontal)]

                + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(5)
                    [
                        SNew(SHorizontalBox)

                            + SHorizontalBox::Slot()
                            .FillWidth(1.0f)
                            .Padding(0, 0, 5, 0)
                            [
                                SAssignNew(SnippetNameInput, SEditableTextBox)
                                    .HintText(LOCTEXT("SnippetHint", "Enter Name"))
                            ]

                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            [
                                SNew(SButton)
                                    .Text(FText::FromString("Create"))
                                    .OnClicked(FOnClicked::CreateRaw(this, &FObserverSnippetsModule::OnCreateSnippetClicked))
                            ]
                    ]
            ];
    }

    // -----------------------------------------------------------------------------

    FReply OnCreateSnippetClicked()
    {
        if(!IsAnyNodeSelectedInEventGraph())
            return FReply::Handled();

        CopyToClipboard();

        FString Clipboard;
        FPlatformApplicationMisc::ClipboardPaste(Clipboard);
        if (!Clipboard.StartsWith(TEXT("Begin Object"))) return FReply::Handled();

        FString Name = SnippetNameInput->GetText().ToString();
        FString Path = FPaths::ProjectSavedDir() / TEXT("Snippets") / (Name + TEXT(".json"));

        TSharedPtr<FJsonObject> Json = MakeShareable(new FJsonObject);
        Json->SetStringField(TEXT("NodeData"), Clipboard);

        FString Output;
        TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Output);
        FJsonSerializer::Serialize(Json.ToSharedRef(), Writer);
        FFileHelper::SaveStringToFile(Output, *Path);

        RefreshSnippetList();

        return FReply::Handled();
    }

    // -----------------------------------------------------------------------------

    bool IsAnyNodeSelectedInEventGraph() const
    {
        UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
        TArray<UObject*> EditedAssets = AssetEditorSubsystem->GetAllEditedAssets();
        for (UObject* Asset : EditedAssets)
        {
            UBlueprint* Blueprint = Cast<UBlueprint>(Asset);
            if (!Blueprint)
                continue;
            IAssetEditorInstance* AssetEditor = AssetEditorSubsystem->FindEditorForAsset(Asset, false);
            FBlueprintEditor* BlueprintEditor = static_cast<FBlueprintEditor*>(AssetEditor);
            if (BlueprintEditor)
            {
                FGraphPanelSelectionSet SelectedNodes = BlueprintEditor->GetSelectedNodes();
                if (SelectedNodes.Num() > 0)
                {
                    UE_LOG(LogObserverSnippets, Log, TEXT("Es wurden %d Nodes im Graph markiert!"), SelectedNodes.Num());
                    return true;
                }
            }
        }
        UE_LOG(LogObserverSnippets, Warning, TEXT("Keine Nodes im EventGraph markiert."));
        return false;
	}

	// -----------------------------------------------------------------------------

    void UpdateCreateButtonState(TSharedPtr<SButton> CreateButton)
    {
        if (CreateButton.IsValid())
        {
            CreateButton->SetEnabled(IsAnyNodeSelectedInEventGraph());
        }
	}

	// -----------------------------------------------------------------------------

    void CopyToClipboard()
    {
        UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
        TArray<UObject*> EditedAssets = AssetEditorSubsystem->GetAllEditedAssets();
        for (UObject* Asset : EditedAssets)
        {
            UBlueprint* Blueprint = Cast<UBlueprint>(Asset);
            if (!Blueprint)
                continue;
            IAssetEditorInstance* AssetEditor = AssetEditorSubsystem->FindEditorForAsset(Asset, false);
            FBlueprintEditor* BlueprintEditor = static_cast<FBlueprintEditor*>(AssetEditor);
            if (BlueprintEditor)
            {
                FGraphPanelSelectionSet SelectedNodes = BlueprintEditor->GetSelectedNodes();
                if (SelectedNodes.Num() > 0)
                {
                    FString ExportedText;
                    FEdGraphUtilities::ExportNodesToText(SelectedNodes, /*out*/ ExportedText);
                    FPlatformApplicationMisc::ClipboardCopy(*ExportedText);
                    UE_LOG(LogObserverSnippets, Log, TEXT("%d Nodes wurden erfolgreich in die Zwischenablage kopiert!"), SelectedNodes.Num());
                }
            }
        }
    }

	// -----------------------------------------------------------------------------

    void PasteFromClipboard()
    {
        UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
        TArray<UObject*> EditedAssets = AssetEditorSubsystem->GetAllEditedAssets();
        for (UObject* Asset : EditedAssets)
        {
            UBlueprint* Blueprint = Cast<UBlueprint>(Asset);
            if (!Blueprint)
                continue;
            IAssetEditorInstance* AssetEditor = AssetEditorSubsystem->FindEditorForAsset(Asset, false);
            FBlueprintEditor* BlueprintEditor = static_cast<FBlueprintEditor*>(AssetEditor);
            if (BlueprintEditor)
            {
                // Wir holen uns den aktuell fokussierten Graph (z.B. EventGraph)
                UEdGraph* CurrentGraph = BlueprintEditor->GetFocusedGraph();
                if (CurrentGraph)
                {
                    // 1. Text aus der Zwischenablage abrufen
                    FString ClipboardText;
                    FPlatformApplicationMisc::ClipboardPaste(ClipboardText);

                    if (!ClipboardText.IsEmpty())
                    {
                        // 2. Transaktion starten (für Undo/Redo Support in UE 5.6)
                        const FScopedTransaction Transaction(FText::FromString("Paste Snippet"));
                        CurrentGraph->Modify();

                        // 3. Die Nodes im aktuellen Graph erzeugen
                        TSet<UEdGraphNode*> PastedNodes;
                        FEdGraphUtilities::ImportNodesFromText(CurrentGraph, ClipboardText, /*out*/ PastedNodes);
                    }
                }
            }
        }
    }

    // -----------------------------------------------------------------------------

    FReply OnSnippetDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, TSharedPtr<FString> SnippetItem)
    {
        if (!SnippetItem.IsValid())
            return FReply::Unhandled();

        FString Name = *SnippetItem;
        FString Path = FPaths::ProjectSavedDir() / TEXT("Snippets") / (Name + TEXT(".json"));
        FString JsonString;

        if (FFileHelper::LoadFileToString(JsonString, *Path))
        {
            TSharedPtr<FJsonObject> Json;
            TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

            if (FJsonSerializer::Deserialize(Reader, Json) && Json.IsValid())
            {
                FString NodeData;
                if (Json->TryGetStringField(TEXT("NodeData"), NodeData))
                {
                    // DragDrop Operation starten
                    return FReply::Handled().BeginDragDrop(FSnippetDragDropAction::New(NodeData));
                }
            }
        }

        return FReply::Unhandled();
    }

    // -----------------------------------------------------------------------------

    void RefreshSnippetList()
    {
        SnippetList.Empty();

        TArray<FString> Files;
        IFileManager::Get().FindFiles(Files, *(FPaths::ProjectSavedDir() / TEXT("Snippets/*.json")), true, false);

        for (FString& File : Files)
        {
            SnippetList.Add(MakeShareable(new FString(FPaths::GetBaseFilename(File))));
        }

        if (SnippetListView.IsValid())
            SnippetListView->RequestListRefresh();
    }

    // -----------------------------------------------------------------------------

    TSharedRef<SWidget> CreateSnippetIcon(FString SnippetName)
    {
        // Pfad zum potenziellen PNG: /Saved/Snippets/Icons/SnippetName.png
        FString IconPath = FPaths::ProjectSavedDir() / TEXT("Snippets/Icons") / (SnippetName + TEXT(".png"));

        const FSlateBrush* DisplayBrush;

        if (FPaths::FileExists(IconPath))
        {
            // Hier müsste man ein dynamisches Brush-Objekt erstellen (External Image)
            // Für den schnellen Start nutzen wir ein UE5-Standard-Icon:
            DisplayBrush = FAppStyle::GetBrush("GraphEditor.Default_16x");
        }
        else
        {
            // Standard-Icon für Blueprints (das kleine weiße/blaue Node-Symbol)
            DisplayBrush = FAppStyle::GetBrush("GraphEditor.Macro_16x");
        }

        return SNew(SImage)
            .Image(DisplayBrush)
            .ColorAndOpacity(FSlateColor(FLinearColor::White));
    }

    // -----------------------------------------------------------------------------

    TSharedRef<ITableRow> GenerateSnippetRow(TSharedPtr<FString> InItem, const TSharedRef<STableViewBase>& OwnerTable)
    {
        return SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
            .OnDragDetected_Lambda([this, InItem](const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) -> FReply
            {
                return OnSnippetDragDetected(MyGeometry, MouseEvent, InItem);
            })
            [
            SNew(SBorder)
                .BorderImage(FAppStyle::GetBrush("NoBorder"))
                .Padding(FMargin(4, 2))
                [
                    SNew(SHorizontalBox)

                        // --- Das Icon ---
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign(VAlign_Center)
                        .Padding(0, 0, 8, 0)
                        [
                            CreateSnippetIcon(*InItem)
                        ]

                        // --- Der Text ---
                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f)
                        .VAlign(VAlign_Center)
                        [
                            SNew(STextBlock)
                                .Text(FText::FromString(*InItem))
                                .Font(FAppStyle::GetFontStyle("PropertyWindow.NormalFont"))
                        ]
                ]
            ];
    }

    // -----------------------------------------------------------------------------

private:

    TSharedPtr<SEditableTextBox> SnippetNameInput;
    TSharedPtr<SListView<TSharedPtr<FString>>> SnippetListView;
    TArray<TSharedPtr<FString>> SnippetList;
};

IMPLEMENT_MODULE(FObserverSnippetsModule, ObserverSnippets);

#undef LOCTEXT_NAMESPACE
