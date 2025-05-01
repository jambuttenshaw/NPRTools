#pragma once

struct FNPRToolsParametersProxy;
struct FNPRToolsHistory;


namespace NPRTools
{
	bool IsEnabled();
	bool IsEnabled_RenderThread();


	// Parameters specify the operations the pipeline will perform
	void ExecuteNPRPipeline(
		FRDGBuilder& GraphBuilder,
		const FNPRToolsParametersProxy& NPRParameters,	// Required - Without parameters pipeline cannot be executed
		FRDGTextureRef InColorTexture,				// Required - Input colour texture to have NPR effects applied
		FRDGTextureRef OutColorTexture,				// Required - Output colour texture will also hold the output processed image
		FNPRToolsHistory* History						// Optional - temporal smoothing will not be available without history
	);

}
