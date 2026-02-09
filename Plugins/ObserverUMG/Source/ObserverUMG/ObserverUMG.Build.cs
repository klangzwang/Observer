namespace UnrealBuildTool.Rules
{
	public class ObserverUMG : ModuleRules
	{
        public ObserverUMG(ReadOnlyTargetRules Target) : base(Target)
		{
			PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

			PublicIncludePaths.AddRange(
			new string[]
			{
				System.IO.Path.Combine(ModuleDirectory, ""),
				System.IO.Path.Combine(ModuleDirectory, "Common"),
				System.IO.Path.Combine(ModuleDirectory, "Common/Activatable"),
				System.IO.Path.Combine(ModuleDirectory, "Common/Activatable/Widget"),
				System.IO.Path.Combine(ModuleDirectory, "Common/Button"),
				System.IO.Path.Combine(ModuleDirectory, "Common/User"),
				System.IO.Path.Combine(ModuleDirectory, "Library"),
				System.IO.Path.Combine(ModuleDirectory, "System")
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
				"GameplayTags",
				"ModularGameplay"
			}
			);

			PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"Slate",
				"SlateCore",
				"EnhancedInput",
				"UMG",
				"CommonUI",
				"CommonInput",
				"ControlFlows",
				"PreLoadScreen",
				"RenderCore",
				"DeveloperSettings",
				"ObserverFramework",
				"ObserverInterfaces",
				"ObserverInput",
				"MoviePlayer"
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
