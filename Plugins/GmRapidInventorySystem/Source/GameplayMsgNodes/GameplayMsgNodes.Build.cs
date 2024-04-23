// Copyright 2024 Dev.GaeMyo. All Rights Reserved.

using UnrealBuildTool;

public class GameplayMsgNodes : ModuleRules
{
	public GameplayMsgNodes(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"KismetCompiler",
				"PropertyEditor",
				"GameplayMsgRuntime",
				"UnrealEd"
			}
		);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"BlueprintGraph",
			}
		);

		PrivateIncludePaths.AddRange(
			new string[]
			{
			}
		);

		PublicIncludePaths.AddRange(
			new string[]
			{
			}
		);
	}
}
