// Copyright Christian Iseli, All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class PMPDBVTarget : TargetRules
{
	public PMPDBVTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("PMPDBV");
	}
}
