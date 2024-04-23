// Copyright 2024 Dev.GaeMyo. All Rights Reserved.

using UnrealBuildTool;

public class GameplayMsgRuntime : ModuleRules
{
	public GameplayMsgRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;				

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"GameplayTags"
			});
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
			});
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			});
	}
}
