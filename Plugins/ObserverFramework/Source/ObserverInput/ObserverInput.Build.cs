namespace UnrealBuildTool.Rules
{
	public class ObserverInput : ModuleRules
	{
        public ObserverInput(ReadOnlyTargetRules Target) : base(Target)
		{
			PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

			PublicIncludePaths.AddRange(
			new string[]
			{
				System.IO.Path.Combine(ModuleDirectory, ""),
				System.IO.Path.Combine(ModuleDirectory, "Component"),
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
				"CoreUObject",
				"Engine",
				"GameplayTags",
				"Slate",
				"SlateCore"
			}
			);

			PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"InputCore",
				"EnhancedInput",
				"CommonUI",
				"CommonInput"
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
