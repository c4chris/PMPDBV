// Copyright Christian Iseli, All Rights Reserved.

using UnrealBuildTool;

public class PMPDBV : ModuleRules
{
	public PMPDBV(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "GeometryCore", "GeometryFramework", "HTTP", "InputCore", "EnhancedInput" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
		PublicDefinitions.Add("WITH_LIBCURL=1");
		PublicDefinitions.Add("CURL_STATICLIB=1");
		string LibCurlPath = Target.UEThirdPartySourceDirectory + "libcurl/8.4.0/include/";
		PublicSystemIncludePaths.Add(LibCurlPath);
		PublicAdditionalLibraries.Add(Target.UEThirdPartySourceDirectory+"libcurl/8.4.0/lib/Win64/Release/libcurl.lib");
		AddEngineThirdPartyPrivateStaticDependencies(Target, new string[] { "nghttp2", "OpenSSL", "zlib" });
		//PublicIncludePaths.Add($"{LibCurlPath}");
		PublicIncludePaths.Add($"PMPDBV/Private/C");
		PublicIncludePaths.Add($"PMPDBV/Private/C/Files");
		PublicIncludePaths.Add($"PMPDBV/Private/C/Headers");
		PublicIncludePaths.Add($"PMPDBV/Private/C/Interface");
	}
}
