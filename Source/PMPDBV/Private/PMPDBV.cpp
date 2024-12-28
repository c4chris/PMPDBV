// Copyright Christian Iseli, All Rights Reserved.

#include "PMPDBV.h"
#include "Modules/ModuleManager.h"
#include "curl/curl.h"
#include "Http.h"
extern "C"
{
  #include "PMPDBV-Bridging-Header.h"
}

#include "Log.h"
#include <Misc/LocalTimestampDirectoryVisitor.h>

void FPMPDBV::StartupModule()
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
	FString RelativePath = FPaths::ProjectContentDir();
	FString AbsolutePath(FPaths::ConvertRelativePathToFull(RelativePath));
	bundleRsrcDir = _strdup(TCHAR_TO_UTF8(*AbsolutePath));
	UE_LOG(LogPMPDBV, Warning, TEXT("bundleRsrcDir is %s %hs"), *AbsolutePath, bundleRsrcDir);
	FString RelativePathD = FPaths::ProjectPersistentDownloadDir();
	FString AbsolutePathD(FPaths::ConvertRelativePathToFull(RelativePathD));
	downloadDir = _strdup(TCHAR_TO_UTF8(*AbsolutePathD));
	UE_LOG(LogPMPDBV, Warning, TEXT("downloadDir is %s %hs"), *AbsolutePathD, downloadDir);
	// downloadDir = strdup(dld.path);
	// tempDir = strdup(tmp);
	printfDelegate = FPMPDBV::PrintfDelegate;
	auto res = InitGlobals();
	//SwiftCBSetup(&callbacks)
	//GL_init_Atom_Colors()
	//ListAllFilesInCurDirectory();
	// Try to load an entry
	auto request = FHttpModule::Get().CreateRequest();
	request->SetURL(FString("https://files.rcsb.org/download/1crn.pdb"));
	request->OnProcessRequestComplete().BindLambda([&](FHttpRequestPtr request, FHttpResponsePtr response, bool success) {
		if (success)
		{
			// FIXME - maybe use GetContent into an array of int8 instead...
			FString content = response->GetContentAsString();
			const FString Filename = TEXT("1crn.pdb");
			const FString FullyQualifiedFileNameFilename = FPaths::Combine(*FPaths::ProjectPersistentDownloadDir(), Filename);
			bool res = FFileHelper::SaveStringToFile(content, *FullyQualifiedFileNameFilename);
			UE_LOG(LogPMPDBV, Log, TEXT("SaveStringToFile %s - %s"), *FullyQualifiedFileNameFilename, res ? TEXT("Succeeeded") : TEXT("Failed"));
			doPDBinput(TCHAR_TO_UTF8(*FullyQualifiedFileNameFilename), TCHAR_TO_UTF8(*Filename));
			OnLoadMsg.Broadcast();
		}
		UE_LOG(LogPMPDBV, Log, TEXT("PMPDBV module HTTP callback - %s"), success ? TEXT("Succeeeded") : TEXT("Failed"));
		});
	request->ProcessRequest();
	UE_LOG(LogPMPDBV, Log, TEXT("PMPDBV module starting up - res of InitGlobals is %d"), (int) res);
}

void FPMPDBV::ShutdownModule()
{
	curl_global_cleanup();
	UE_LOG(LogPMPDBV, Log, TEXT("PMPDBV module shutting down"));
}

int FPMPDBV::PrintfDelegate(const char* fmt, va_list va)
{
	TCHAR buf[4096];
	size_t n = strlen(fmt);
	const FString& Str = FString(n, fmt);
	const FString& Str2 = Str.Replace(TEXT("%s"), TEXT("%hs"));
	const TCHAR* Text = *Str2;
	FCString::GetVarArgs(buf, 4096, Text, va);
	UE_LOG(LogPMPDBV, Log, TEXT("from printf : %s"), buf);
	return 0;
}

/**
Gets all the files in a given directory.
@param directory The full path of the directory we want to iterate over.
@param fullpath Whether the returned list should be the full file paths or just the filenames.
@param onlyFilesStartingWith Will only return filenames starting with this string. Also applies onlyFilesEndingWith if specified.
@param onlyFilesEndingWith Will only return filenames ending with this string (it looks at the extension as well!). Also applies onlyFilesStartingWith if specified.
@return A list of files (including the extension).
*/
void FPMPDBV::ListAllFilesInCurDirectory(void)
{
	// Get all files in directory
	TArray<FString> directoriesToSkip;
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FLocalTimestampDirectoryVisitor Visitor(PlatformFile, directoriesToSkip, directoriesToSkip, false);
	FString RelativePath = FPaths::ProjectContentDir() + TEXT("Misc/");

	UE_LOG(LogPMPDBV, Log, TEXT("RelativePath: %s"), *RelativePath);
	//GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Red, RelativePath);

	FString CollapsedPath(RelativePath);
	bool bCollapseSuccess = FPaths::CollapseRelativeDirectories(CollapsedPath);
	FString CollapsedSuccess = bCollapseSuccess ? FString(TEXT("TRUE")) : FString(TEXT("FALSE"));
	UE_LOG(LogPMPDBV, Warning, TEXT("CollapsedPath: %s"), *CollapsedPath);
	//GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Emerald, CollapsedPath);
	UE_LOG(LogPMPDBV, Warning, TEXT("CollapsedSuccess: %s"), *CollapsedSuccess);
	//GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Orange, CollapsedSuccess);

	FString AbsolutePath(FPaths::ConvertRelativePathToFull(RelativePath));
	UE_LOG(LogPMPDBV, Warning, TEXT("AbsolutePath: %s"), *AbsolutePath);
	//GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Cyan, AbsolutePath);
	PlatformFile.IterateDirectory(*RelativePath, Visitor);

	for (TMap<FString, FDateTime>::TIterator TimestampIt(Visitor.FileTimes); TimestampIt; ++TimestampIt)
	{
		const FString filePath = TimestampIt.Key();
		const FString fileName = FPaths::GetCleanFilename(filePath);
		UE_LOG(LogPMPDBV, Log, TEXT("Got file %s at %s"), *FString(fileName), *FString(filePath));
	}
}

IMPLEMENT_PRIMARY_GAME_MODULE( FPMPDBV, PMPDBV, "PMPDBV" );
