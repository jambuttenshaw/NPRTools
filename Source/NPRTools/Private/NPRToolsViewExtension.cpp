#include "NPRToolsViewExtension.h"

#include "ShaderParameterStruct.h"
#include "ScreenPass.h"

#include "PostProcess/PostProcessInputs.h"
#include "SceneTextures.h"
#include "FXRenderingUtils.h"


class FSobelPassPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FSobelPassPS);
	SHADER_USE_PARAMETER_STRUCT(FSobelPassPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)

		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, ViewPort)

		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, SceneColor)

		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()
};

IMPLEMENT_GLOBAL_SHADER(FSobelPassPS, "/NPRTools/Sobel.usf", "SobelPassPS", SF_Pixel);


FNPRToolsViewExtension::FNPRToolsViewExtension(const FAutoRegister& AutoRegister)
	: FSceneViewExtensionBase(AutoRegister)
{
}

void FNPRToolsViewExtension::PrePostProcessPass_RenderThread(
	FRDGBuilder& GraphBuilder, 
	const FSceneView& View, 
	const FPostProcessingInputs& Inputs)
{
	// Get the scene colour texture from the post process inputs
	Inputs.Validate();
	FRDGTextureRef SceneColor = (*Inputs.SceneTextures)->SceneColorTexture;

	// Create a texture to hold the output of our Sobel filter
	// It should be the same format etc as the scene colour texture
	FRDGTextureDesc SobelOutputDesc = SceneColor->Desc;
	SobelOutputDesc.ClearValue = FClearValueBinding(FLinearColor(0.0f, 0.0f, 0.0f));

	FRDGTextureRef SobelOutputTexture = GraphBuilder.CreateTexture(SobelOutputDesc, TEXT("SobelOutput"));

	// We want to perform our postprocessing to the entire viewport
	FScreenPassTextureViewport ViewPort(UE::FXRenderingUtils::GetRawViewRectUnsafe(View));

	// Perform sobel filter pass
	{
		// Set up shader parameters
		FSobelPassPS::FParameters* PassParameters = GraphBuilder.AllocParameters<FSobelPassPS::FParameters>();

		PassParameters->View = View.ViewUniformBuffer;
		PassParameters->ViewPort = GetScreenPassTextureViewportParameters(ViewPort);

		PassParameters->SceneColor = GraphBuilder.CreateSRV(SceneColor);
		PassParameters->RenderTargets[0] = FRenderTargetBinding(SobelOutputTexture, ERenderTargetLoadAction::EClear);

		// Retrieve our pixel shader from the global shader map
		const FGlobalShaderMap* ShaderMap = GetGlobalShaderMap(View.GetFeatureLevel());
		TShaderMapRef<FSobelPassPS> PixelShader(ShaderMap);

		// Add a full-screen pass to the render graph
		AddDrawScreenPass(
			GraphBuilder,
			RDG_EVENT_NAME("SobelFilter"),
			View,
			ViewPort,
			ViewPort,
			PixelShader,
			PassParameters
		);
	}

	// Copy our output back to the back buffer
	{
		AddCopyTexturePass(
			GraphBuilder,
			SobelOutputTexture,
			SceneColor
		);
	}
}
