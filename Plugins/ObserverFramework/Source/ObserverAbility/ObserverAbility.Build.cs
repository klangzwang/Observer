namespace UnrealBuildTool.Rules
{
	public class ObserverAbility : ModuleRules
	{
        public ObserverAbility(ReadOnlyTargetRules Target) : base(Target)
		{
			PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

			PublicIncludePaths.AddRange(
			new string[]
			{
				System.IO.Path.Combine(ModuleDirectory, ""),
				System.IO.Path.Combine(ModuleDirectory, "Abilities"),
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
				"GameplayTags",
				"GameplayAbilities",
				"GameplayTasks"
			}
			);

			PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine"
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
