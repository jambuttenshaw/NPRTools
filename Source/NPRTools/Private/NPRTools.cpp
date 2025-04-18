// Copyright Epic Games, Inc. All Rights Reserved.

#include "NPRTools.h"

#define LOCTEXT_NAMESPACE "FNPRToolsModule"

DEFINE_LOG_CATEGORY(LogNPRTools);


void FNPRToolsModule::StartupModule()
{
	// Map our shader directory
	FString BaseDir = FPaths::Combine(FPaths::ProjectPluginsDir(), TEXT("NPRTools"));
	FString ModuleShaderDir = FPaths::Combine(BaseDir, TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/NPRTools"), ModuleShaderDir);
}

void FNPRToolsModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FNPRToolsModule, NPRTools)