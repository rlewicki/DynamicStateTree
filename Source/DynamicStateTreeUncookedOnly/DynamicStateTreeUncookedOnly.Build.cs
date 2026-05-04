// Copyright 2026, Robert Lewicki, All rights reserved.

using UnrealBuildTool;

public class DynamicStateTreeUncookedOnly : ModuleRules
{
	public DynamicStateTreeUncookedOnly(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", 
				"CoreUObject",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"BlueprintGraph",
				"Engine",
				"StateTreeModule",
				"DynamicStateTree",
				"KismetCompiler",
				"UnrealEd",
			}
		);
	}
}