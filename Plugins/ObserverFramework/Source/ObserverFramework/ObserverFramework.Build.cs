namespace UnrealBuildTool.Rules
{
	public class ObserverFramework : ModuleRules
	{
        public ObserverFramework(ReadOnlyTargetRules Target) : base(Target)
		{
			PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

			PublicIncludePaths.AddRange(
			new string[]
			{
				System.IO.Path.Combine(ModuleDirectory, ""),
				System.IO.Path.Combine(ModuleDirectory, "Actor"),
				System.IO.Path.Combine(ModuleDirectory, "Animation"),
				System.IO.Path.Combine(ModuleDirectory, "Component"),
				System.IO.Path.Combine(ModuleDirectory, "Device"),
				System.IO.Path.Combine(ModuleDirectory, "Game"),
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
				"CoreUObject",
				"ApplicationCore",
				"Engine",
				"PhysicsCore",
				"GameplayTags",
				"AIModule",
				"Niagara",
				"MovieScene",
				"LevelSequence",
				"ModularGameplay",
				"NavigationSystem",
				"CinematicCamera"
			}
			);

			PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"InputCore",
				"Slate",
				"SlateCore",
				"RenderCore",
				"DeveloperSettings",
				"EnhancedInput",
				"UMG",
				"NetCore",
				"CommonUI",
				"CommonInput",
				"ControlFlows",
				"AudioMixer",
				"AudioModulation",
				"ObserverInterfaces",
				"ObserverInput"
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
