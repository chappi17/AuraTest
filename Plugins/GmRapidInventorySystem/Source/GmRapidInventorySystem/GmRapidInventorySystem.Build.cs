// Copyright Dev.GaeMyo 2024. All Rights Reserved.

using UnrealBuildTool;

public class GmRapidInventorySystem : ModuleRules
{
	public GmRapidInventorySystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivatePCHHeaderFile = "Public/GmRapidInventorySystem.h";
		
		PublicIncludePaths.AddRange(
			new string[] {
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"UMG",
				"CommonUI",
				"DeveloperSettings",
				"GameplayAbilities",
				"GameplayTasks",
				"GameplayTags",
				"ReplicationGraph",
				"ModularGameplay",
				"EnhancedInput"
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Slate",
				"SlateCore",
				"NetCore",
				"GameplayMsgRuntime"
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);
		
		SetupGameplayDebuggerSupport(Target);
		SetupIrisSupport(Target);
	}
}
