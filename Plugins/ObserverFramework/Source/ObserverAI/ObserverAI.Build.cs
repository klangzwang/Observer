namespace UnrealBuildTool.Rules
{
	public class ObserverAI : ModuleRules
	{
        public ObserverAI(ReadOnlyTargetRules Target) : base(Target)
		{
			PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

			PublicIncludePaths.AddRange(
			new string[]
			{
				System.IO.Path.Combine(ModuleDirectory, ""),
				System.IO.Path.Combine(ModuleDirectory, "Actor"),
				System.IO.Path.Combine(ModuleDirectory, "Component")
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
				"ModularGameplay",
				"GameplayTags",
				"AIModule"
			}
			);

			PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"GameplayTasks",
				"NavigationSystem",
				"AIModule",
				"ObserverFramework",
				"ObserverInterfaces"
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
