#include "NPRToolsWorldSubsystem.h"

#include "SceneViewExtension.h"
#include "NPRToolsViewExtension.h"


void UNPRToolsWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	// View extension lifetime is equal to the UWorld lifetime
	NPRToolsViewExtension = FSceneViewExtensions::NewExtension<FNPRToolsViewExtension>();
}

void UNPRToolsWorldSubsystem::Deinitialize()
{
	
}
