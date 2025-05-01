#include "NPRToolsViewExtension.h"

#include "NPRTools.h"
#include "NPRToolsWorldSubsystem.h"
#include "NPRToolsParametersProxy.h"
#include "NPRToolsHistory.h"
#include "Pipelines/NPRPipeline.h"

#include "ShaderParameterStruct.h"
#include "ScreenPass.h"

#include "PostProcess/PostProcessInputs.h"
#include "RenderTargetPool.h"
#include "SceneTextures.h"



FNPRToolsViewExtension::FNPRToolsViewExtension(const FAutoRegister& AutoRegister, UNPRToolsWorldSubsystem* InWorldSubsystem)
	: FSceneViewExtensionBase(AutoRegister)
	, WorldSubsystem(InWorldSubsystem)
{
	History = MakeUnique<FNPRToolsHistory>();
}

void FNPRToolsViewExtension::BeginRenderViewFamily(FSceneViewFamily& InViewFamily)
{
	if (IsValid(WorldSubsystem))
		WorldSubsystem->TransferState();
}

bool FNPRToolsViewExtension::IsActiveThisFrame_Internal(const FSceneViewExtensionContext& Context) const
{
	return NPRTools::IsEnabled() && IsValid(WorldSubsystem);
}


void FNPRToolsViewExtension::PrePostProcessPass_RenderThread(
	FRDGBuilder& GraphBuilder, 
	const FSceneView& View, 
	const FPostProcessingInputs& Inputs)
{
	if (!IsValid(WorldSubsystem) || !WorldSubsystem->ParametersProxy.IsValid())
		return;
	FNPRToolsParametersProxy* Parameters = WorldSubsystem->ParametersProxy.Get();
	
	// Get the scene colour texture from the post process inputs
	Inputs.Validate();
	FRDGTextureRef SceneColorTexture = (*Inputs.SceneTextures)->SceneColorTexture;

	NPRTools::ExecuteNPRPipeline(
		GraphBuilder,
		View,
		*Parameters,
		SceneColorTexture,
		History.Get()
	);
 }
