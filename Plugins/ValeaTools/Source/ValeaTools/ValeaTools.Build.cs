using UnrealBuildTool;

public class ValeaTools : ModuleRules
{
	public ValeaTools(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
		new string[]
		{
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
			"ApplicationCore", 
			"ToolMenus",
			"PythonScriptPlugin"
		}
		);

		PrivateDependencyModuleNames.AddRange(
		new string[]
		{
			"Slate",
			"SlateCore",
            "EditorFramework",
            "UnrealEd",
			"LevelEditor",
            "WorkspaceMenuStructure",
            "ToolMenus",
            "Projects",
			"Kismet",
			"BlueprintGraph",
			"GraphEditor",
			"EditorStyle",
			"AppFramework",
			"AssetDefinition",
		}
		);

		DynamicallyLoadedModuleNames.AddRange(
		new string[]
		{
		}
		);
	}
}
