// Copyright Christian Iseli, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

DECLARE_MULTICAST_DELEGATE(FLoadPDBDispatcher);

class FPMPDBV : public IModuleInterface
{
public:
	static inline FPMPDBV& Get()
	{
		return FModuleManager::LoadModuleChecked<FPMPDBV>("PMPDBV");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("PMPDBV");
	}

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static int PrintfDelegate(const char* fmt, va_list va);
	void ListAllFilesInCurDirectory(void);

	UPROPERTY(BlueprintAssignable)
	FLoadPDBDispatcher OnLoadMsg;
};
