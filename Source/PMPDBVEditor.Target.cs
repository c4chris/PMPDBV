// Copyright Christian Iseli, All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class PMPDBVEditorTarget : TargetRules
{
	public PMPDBVEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("PMPDBV");
	}
}
