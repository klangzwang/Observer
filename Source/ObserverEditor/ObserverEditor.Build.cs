using UnrealBuildTool;

public class ObserverEditor : ModuleRules
{
    public ObserverEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[]
			{
				"ObserverEditor"
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
                "EditorFramework",
                "UnrealEd",
				"PhysicsCore",
				"StudioTelemetry",
				"ObserverGame",
				"ApplicationCore"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
			{
				"InputCore",
				"Slate",
				"SlateCore",
				"ToolMenus",
				"EditorStyle",
				"DataValidation",
				"MessageLog",
				"Projects",
				"DeveloperToolSettings",
				"CollectionManager",
				"SourceControl",
				"Chaos",
				"PythonScriptPlugin",
				"Blutility",
				"UMG",
				"UMGEditor",
				"MoviePlayer",
				"Json",
				"JsonUtilities",
				"GraphEditor"
			}
        );

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
		);
    }
}
