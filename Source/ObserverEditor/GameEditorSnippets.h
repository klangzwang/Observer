#pragma once
#include "Widgets/Docking/SDockTab.h"

class FGameEditorSnippets
{
	
public:

	static TSharedRef<SDockTab> OnSpawnTab(const FSpawnTabArgs& SpawnTabArgs);

    static const FName SnippetsTabName;
};
