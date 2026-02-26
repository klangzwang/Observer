#include "Modules/ModuleManager.h"
#include "BlueprintEditorModule.h"
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

#define LOCTEXT_NAMESPACE "ValeaTools"

//
//
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

class FValeaToolsModule : public IModuleInterface
{
    typedef FValeaToolsModule ThisClass;

public:

    virtual void StartupModule() override
    {
        FGlobalTabmanager::Get()->RegisterNomadTabSpawner(TabId, FOnSpawnTab::CreateRaw(this, &FValeaToolsModule::OnSpawnTab))
            .SetDisplayName(LOCTEXT("TabTitle", "ValeaTools"))
            .SetMenuType(ETabSpawnerMenuType::Hidden);

        FBlueprintEditorModule& BlueprintEditorModule = FModuleManager::LoadModuleChecked<FBlueprintEditorModule>("Kismet");

        TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());

        MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, nullptr, FMenuExtensionDelegate::CreateRaw(this, &FValeaToolsModule::OnExtendMenu));

        BlueprintEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);

        RefreshSnippetList();
    }

    virtual void ShutdownModule() override
    {
        FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TabId);
    }

private:

    const FLinearColor ColorBackground = FLinearColor(0.08f, 0.09f, 0.1f, 1.0f); // Dunkles Anthrazit
    const FLinearColor ColorAccentOrange = FLinearColor(0.95f, 0.45f, 0.2f, 1.0f); // Das leuchtende Orange
    const FLinearColor ColorButtonNormal = FLinearColor(0.12f, 0.13f, 0.15f, 1.0f); // Etwas heller für Tiefe

    TSharedRef<SDockTab> OnSpawnTab(const FSpawnTabArgs& SpawnTabArgs)
    {
        return SNew(SDockTab)
            .TabRole(ETabRole::NomadTab)
            [
                // Hauptcontainer mit dem dunklen Hintergrund der Vorlage
                SNew(SBorder)
                    .BorderImage(FAppStyle::GetBrush("WhiteBrush"))
                    .BorderBackgroundColor(ColorBackground)
                    .Padding(12.0f)
                    [
                        SNew(SVerticalBox)

                            // --- HEADER: "ValeaTools" ---
                            + SVerticalBox::Slot()
                            .AutoHeight()
                            .Padding(0, 0, 0, 15)
                            [
                                SNew(STextBlock)
                                    .Text(LOCTEXT("TitleText", "VALEATOOLS"))
                                    .Font(FAppStyle::GetFontStyle("BoldFont"))
                                    .ColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.5f, 0.5f, 1.0f)))
                            ]

                            // --- INPUT BEREICH (Textfeld & Create Button) ---
                            + SVerticalBox::Slot()
                            .AutoHeight()
                            .Padding(0, 5)
                            [
                                SNew(SHorizontalBox)
                                    + SHorizontalBox::Slot()
                                    .FillWidth(1.0f)
                                    .Padding(0, 0, 8, 0)
                                    [
                                        SNew(SBorder) // Äußerer "Schatten" Effekt für Neumorphism
                                            .BorderImage(FAppStyle::GetBrush("WhiteBrush"))
                                            .BorderBackgroundColor(FLinearColor(0.05f, 0.05f, 0.06f, 1.0f))
                                            .Padding(2.0f)
                                            [
                                                SAssignNew(SnippetNameInput, SEditableTextBox)
                                                    .BackgroundColor(ColorButtonNormal)
                                                    .ForegroundColor(FLinearColor::White)
                                                    .HintText(LOCTEXT("SnippetHint", "Snippet Name..."))
                                                    .Style(&FAppStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("NormalEditableTextBox"))
                                            ]
                                    ]
                                + SHorizontalBox::Slot()
                                    .AutoWidth()
                                    [
                                        SNew(SButton)
                                            .ButtonStyle(FAppStyle::Get(), "PrimaryButton") // Orangefarbener Akzent wie im "Active" State
                                            .ButtonColorAndOpacity(ColorAccentOrange)
                                            .OnClicked(FOnClicked::CreateRaw(this, &FValeaToolsModule::OnCreateSnippetClicked))
                                            [
                                                SNew(STextBlock)
                                                    .Text(FText::FromString("+"))
                                                    .Font(FAppStyle::GetFontStyle("BoldFont"))
                                                    .ColorAndOpacity(FLinearColor::White)
                                                    .Margin(FMargin(10, 2))
                                            ]
                                    ]
                            ]

                        // --- LISTE DER SNIPPETS ---
                        + SVerticalBox::Slot()
                            .FillHeight(1.0f)
                            .Padding(0, 15, 0, 0)
                            [
                                SNew(SBorder)
                                    .BorderImage(FAppStyle::GetBrush("WhiteBrush"))
                                    .BorderBackgroundColor(FLinearColor(0.06f, 0.07f, 0.08f, 1.0f))
                                    .Padding(4.0f)
                                    [
                                        SAssignNew(SnippetListView, SListView<TSharedPtr<FString>>)
                                            .ListItemsSource(&SnippetList)
                                            .OnGenerateRow_Raw(this, &FValeaToolsModule::GenerateSnippetRow)
                                            .SelectionMode(ESelectionMode::Single)
                                    ]
                            ]

                        // --- FOOTER (Signatur) ---
                        + SVerticalBox::Slot()
                            .AutoHeight()
                            .HAlign(HAlign_Right)
                            .Padding(0, 10, 0, 0)
                            [
                                SNew(STextBlock)
                                    .Text(FText::FromString("Vinny Singh")) // Hommage an das Design
                                    .Font(FAppStyle::GetFontStyle("DetailsView.CategoryFontStyle"))
                                    .ColorAndOpacity(FSlateColor(FLinearColor(0.3f, 0.3f, 0.3f, 1.0f)))
                            ]
                    ]
            ];
    }

private:

	const FName TabId = FName("ValeaToolsTab");

    // -----------------------------------------------------------------------------

    void OnExtendMenu(FMenuBuilder& MenuBuilder)
    {
        MenuBuilder.BeginSection("ValeaTools", LOCTEXT("ValeaToolsHeader", "ValeaTools"));
        {
            MenuBuilder.AddMenuEntry(
                LOCTEXT("SnippetsTitle", "Snippets"),
                LOCTEXT("SnippetsTooltip", "Open the Snippet Tap Window"),
                FSlateIcon(),
                FUIAction(FExecuteAction::CreateRaw(this, &FValeaToolsModule::ExecuteWindow))
            );
        }
        MenuBuilder.EndSection();
    }

    // -----------------------------------------------------------------------------

    void ExecuteWindow()
    {
        FBlueprintEditorModule& BlueprintEditorModule = FModuleManager::LoadModuleChecked<FBlueprintEditorModule>("Kismet");
        FGlobalTabmanager::Get()->TryInvokeTab(TabId);
    }

    // -----------------------------------------------------------------------------

    FReply OnCreateSnippetClicked()
    {
        if (!IsAnyNodeSelectedInEventGraph())
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
                    UE_LOG(LogTemp, Log, TEXT("Es wurden %d Nodes im Graph markiert!"), SelectedNodes.Num());
                    return true;
                }
            }
        }
        UE_LOG(LogTemp, Warning, TEXT("Keine Nodes im EventGraph markiert."));
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
                    UE_LOG(LogTemp, Log, TEXT("%d Nodes wurden erfolgreich in die Zwischenablage kopiert!"), SelectedNodes.Num());
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

    FReply OnDeleteSnippetClicked(TSharedPtr<FString> SnippetName)
    {
        if (!SnippetName.IsValid())
            return FReply::Unhandled();

        FString Path = FPaths::ProjectSavedDir() / TEXT("Snippets") / (*SnippetName + TEXT(".json"));

        if (IFileManager::Get().Delete(*Path))
        {
            UE_LOG(LogTemp, Log, TEXT("Snippet %s wurde gelöscht."), **SnippetName);
            RefreshSnippetList();
        }

        return FReply::Handled();
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

                            // --- Der Lösch-Button (Rechte Seite) ---
                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            .HAlign(HAlign_Right)
                            .VAlign(VAlign_Center)
                            .Padding(2)
                            [
                                SNew(SButton)
                                    .ButtonStyle(FAppStyle::Get(), "SimpleButton") // Dezenter Button
                                    .ContentPadding(2)
                                    .OnClicked(FOnClicked::CreateRaw(this, &FValeaToolsModule::OnDeleteSnippetClicked, InItem))
                                    .ToolTipText(LOCTEXT("DeleteTooltip", "Snippet löschen"))
                                    [
                                        SNew(SImage)
                                            .Image(FAppStyle::GetBrush("Icons.Delete")) // Das "X" Icon
                                            .ColorAndOpacity(FSlateColor(FLinearColor(1, 0.2, 0.2, 1))) // Leicht rötlich
                                    ]
                            ]
                    ]
            ];
    }

private:

    TSharedPtr<SEditableTextBox> SnippetNameInput;
    TSharedPtr<SListView<TSharedPtr<FString>>> SnippetListView;
    TArray<TSharedPtr<FString>> SnippetList;
};

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FValeaToolsModule, ValeaTools)
