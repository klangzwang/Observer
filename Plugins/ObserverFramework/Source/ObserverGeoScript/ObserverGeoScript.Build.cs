namespace UnrealBuildTool.Rules
{
	public class ObserverGeoScript : ModuleRules
	{
        public ObserverGeoScript(ReadOnlyTargetRules Target) : base(Target)
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
				"GeometryScriptingCore"
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
