#include "NPRToolsViewExtension.h"


FNPRToolsViewExtension::FNPRToolsViewExtension(const FAutoRegister& AutoRegister)
	: FSceneViewExtensionBase(AutoRegister)
{
	UE_LOG(LogTemp, Display, TEXT("Created NPR tools view extension."));
}

void FNPRToolsViewExtension::PrePostProcessPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessingInputs& Inputs)
{
	// Perform rendering work
	UE_LOG(LogTemp, Display, TEXT("Performing pre-post-process pass NPR rendering work."));
}
