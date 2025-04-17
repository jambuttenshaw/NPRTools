#pragma once

#include "SceneViewExtension.h"

class UNPRToolsWorldSubsystem;


class FNPRToolsViewExtension : public FSceneViewExtensionBase
{
public:
	FNPRToolsViewExtension(const FAutoRegister& AutoRegister, UNPRToolsWorldSubsystem* InWorldSubsystem);

	//~ Begin FSceneViewExtensionBase Interface
	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override {};
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override {};
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override;

	virtual bool IsActiveThisFrame_Internal(const FSceneViewExtensionContext& Context) const override;

	virtual void PrePostProcessPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessingInputs& Inputs) override;
	//~ End FSceneViewExtensionBase Interface

public:
	// Called when view extension is released from the subsystem (view extension lifetime may be prolonged by render thread)
	void Invalidate() { WorldSubsystem = nullptr; }

private:
	UNPRToolsWorldSubsystem* WorldSubsystem;

};
