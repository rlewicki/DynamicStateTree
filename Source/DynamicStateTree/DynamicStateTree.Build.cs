// Copyright 2026, Robert Lewicki, All rights reserved.

using UnrealBuildTool;

public class DynamicStateTree : ModuleRules
{
	public DynamicStateTree(ReadOnlyTargetRules Target) : base(Target)
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
				"Engine",
				"StateTreeModule",
			}
		);
	}
}