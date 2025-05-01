#include "NPRToolsViewExtension.h"

#include "NPRToolsWorldSubsystem.h"
#include "NPRToolsHistory.h"
#include "Pipelines/NPRPipeline.h"

#include "PostProcess/PostProcessInputs.h"
#include "RenderTargetPool.h"
#include "SceneTextures.h"



FNPRToolsViewExtension::FNPRToolsViewExtension(const FAutoRegister& AutoRegister, UNPRToolsWorldSubsystem* InWorldSubsystem)
	: FSceneViewExtensionBase(AutoRegister)
	, WorldSubsystem(InWorldSubsystem)
{
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
	if (View.bIsSceneCapture && !NPRTools::IsEnabledForSceneCaptures_RenderThread())
		return;

	if (!IsValid(WorldSubsystem) || !WorldSubsystem->ParametersProxy.IsValid())
		return;
	FNPRToolsParametersProxy* Parameters = WorldSubsystem->ParametersProxy.Get();
	
	// Get the scene colour texture from the post process inputs
	Inputs.Validate();
	FRDGTextureRef SceneColorTexture = (*Inputs.SceneTextures)->SceneColorTexture;

	FRDGTextureDesc TextureDesc = SceneColorTexture->Desc;
	TextureDesc.ClearValue = FClearValueBinding(FLinearColor(0.0f, 0.0f, 0.0f));
	TextureDesc.Format = PF_FloatRGBA;

	FRDGTextureRef OutTexture = GraphBuilder.CreateTexture(TextureDesc, TEXT("NPRTools.Result"));
	AddClearRenderTargetPass(GraphBuilder, OutTexture);

	bool bSuccess = NPRTools::ExecuteNPRPipeline(
		GraphBuilder,
		*Parameters,
		SceneColorTexture,
		OutTexture,
		nullptr
	);

	if (bSuccess)
	{
		// Copy output back to scene color for remainder of scene rendering
		AddCopyTexturePass(GraphBuilder, OutTexture, SceneColorTexture);
	}
 }
