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
				"UMGEditor"
			}
        );

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
		);
    }
}
