#pragma once

struct FNPRToolsParametersProxy;
struct FNPRToolsHistory;


namespace NPRTools
{
	bool IsEnabled();
	bool IsEnabled_RenderThread();


	// Pipeline operates in place - input texture will be modified directly
	// Parameters specify the operations the pipeline will perform
	void ExecuteNPRPipeline(
		FRDGBuilder& GraphBuilder,
		const FSceneView& View,
		const FNPRToolsParametersProxy& NPRParameters,	// Required - Without parameters pipeline cannot be executed
		FRDGTextureRef InOutColorTexture,				// Required - Input colour texture, and will also hold the output processed image
		FNPRToolsHistory* History						// Optional - temporal smoothing will not be available without history
	);

}
