// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GSD2_Project : ModuleRules
{
	public GSD2_Project(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"GSD2_Project",
			"GSD2_Project/Variant_Horror",
			"GSD2_Project/Variant_Horror/UI",
			"GSD2_Project/Variant_Shooter",
			"GSD2_Project/Variant_Shooter/AI",
			"GSD2_Project/Variant_Shooter/UI",
			"GSD2_Project/Variant_Shooter/Weapons"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
