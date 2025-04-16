#pragma once

#include "SceneViewExtension.h"


class FNPRToolsViewExtension : public FSceneViewExtensionBase
{
public:
	FNPRToolsViewExtension(const FAutoRegister& AutoRegister);

	//~ Begin FSceneViewExtensionBase Interface
	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override {};
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override {};
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override {};

	virtual void PrePostProcessPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessingInputs& Inputs) override;
	//~ End FSceneViewExtensionBase Interface

private:

};
