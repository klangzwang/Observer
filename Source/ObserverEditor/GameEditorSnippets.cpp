#include "GameEditorSnippets.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "LevelEditor.h"

#define LOCTEXT_NAMESPACE "GameEditorSnippets"

const FName FGameEditorSnippets::SnippetsTabName("Snippets");

TSharedRef<SDockTab> FGameEditorSnippets::OnSpawnTab(const FSpawnTabArgs& SpawnTabArgs)
{
    // Hier definieren wir das visuelle Layout (Slate)
    return SNew(SDockTab)
        .TabRole(ETabRole::NomadTab)
        [
            SNew(SBox)
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            [
                // Platzhalter für deine zukünftige Snippet-Liste
                SNew(STextBlock)
                .Text(FText::FromString("Hier entstehen deine Snippets!"))
            ]
        ];
}

#undef LOCTEXT_NAMESPACE
