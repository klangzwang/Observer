namespace UnrealBuildTool.Rules
{
	public class ObserverMovie : ModuleRules
	{
        public ObserverMovie(ReadOnlyTargetRules Target) : base(Target)
		{
			PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

			PublicIncludePaths.AddRange(
			new string[]
			{
				System.IO.Path.Combine(ModuleDirectory, "Private")
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
			}
			);

			PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"MoviePlayer",
				"Slate",
				"SlateCore",
				"InputCore"
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
