namespace UnrealBuildTool.Rules
{
	public class ObserverSnippets : ModuleRules
	{
        public ObserverSnippets(ReadOnlyTargetRules Target) : base(Target)
		{
			PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

			PublicIncludePaths.AddRange(
			new string[]
			{
				System.IO.Path.Combine(ModuleDirectory, "")
			}
			);

			PrivateIncludePaths.AddRange(
			new string[]
			{
			}
			);

			PublicDependencyModuleNames.AddRange(
			new string[]
			{
                "Core",
                "CoreUObject",
                "Engine",
				"InputCore",
				"Json",
				"JsonUtilities",
				"ApplicationCore"
			}
			);

			PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Slate",
                "SlateCore",
                "UnrealEd",
                "LevelEditor",
                "ToolMenus",
                "WorkspaceMenuStructure",
				"GraphEditor",
				"BlueprintGraph",
				"Kismet",
				"EditorStyle",
				"AppFramework",
				"AssetDefinition"
			}
			);
				
			DynamicallyLoadedModuleNames.AddRange(
			new string[]
				{
				}
			);
		}
	}
}
