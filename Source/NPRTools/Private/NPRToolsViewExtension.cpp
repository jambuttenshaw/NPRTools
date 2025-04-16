#include "NPRToolsViewExtension.h"

#include "ShaderParameterStruct.h"
#include "ScreenPass.h"

#include "PostProcess/PostProcessInputs.h"
#include "SceneTextures.h"


static TAutoConsoleVariable<bool> CVarNPRToolsEnable(
	TEXT("npr.Enable"),
	true,
	TEXT("Enables NPR pipeline (Default = true)"),
	ECVF_RenderThreadSafe
);

namespace NPRTools
{
	static bool IsEnabled()
	{
		return CVarNPRToolsEnable.GetValueOnRenderThread();
	}
}


class FSobelPassPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FSobelPassPS);
	SHADER_USE_PARAMETER_STRUCT(FSobelPassPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)

		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, ViewPort)

		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, SceneColorTexture)

		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()
};

IMPLEMENT_GLOBAL_SHADER(FSobelPassPS, "/NPRTools/Sobel.usf", "SobelPS", SF_Pixel);


class FBlurEigenHorizontalPassPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FBlurEigenHorizontalPassPS);
	SHADER_USE_PARAMETER_STRUCT(FBlurEigenHorizontalPassPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)

		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, ViewPort)

		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, InTexture)

		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()
};

IMPLEMENT_GLOBAL_SHADER(FBlurEigenHorizontalPassPS, "/NPRTools/EigenBlur.usf", "BlurEigenVerticalPS", SF_Pixel);


class FBlurEigenVerticalPassPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FBlurEigenVerticalPassPS);
	SHADER_USE_PARAMETER_STRUCT(FBlurEigenVerticalPassPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)

		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, ViewPort)

		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, InTexture)

		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()
};

IMPLEMENT_GLOBAL_SHADER(FBlurEigenVerticalPassPS, "/NPRTools/EigenBlur.usf", "BlurEigenHorizontalPS", SF_Pixel);


class FConvertYCCPassPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FConvertYCCPassPS);
	SHADER_USE_PARAMETER_STRUCT(FConvertYCCPassPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)

		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, ViewPort)

		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, SceneColorTexture)

		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()
};

IMPLEMENT_GLOBAL_SHADER(FConvertYCCPassPS, "/NPRTools/ConvertYCC.usf", "ConvertYCbCrPS", SF_Pixel);


class FBilateralPassPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FBilateralPassPS);
	SHADER_USE_PARAMETER_STRUCT(FBilateralPassPS, FGlobalShader);

	class FBilateralDirectionTangent : SHADER_PERMUTATION_BOOL("BILATERAL_DIRECTION_TANGENT");
	using FPermutationDomain = TShaderPermutationDomain<FBilateralDirectionTangent>;

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)

		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, ViewPort)

		SHADER_PARAMETER(float, SigmaD)
		SHADER_PARAMETER(float, SigmaR)

		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, InSceneColorYCCTexture)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, InTangentFlowMapTexture)

		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()
};

IMPLEMENT_GLOBAL_SHADER(FBilateralPassPS, "/NPRTools/Bilateral.usf", "BilateralPS", SF_Pixel);


class FDoGGradientPassPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FDoGGradientPassPS);
	SHADER_USE_PARAMETER_STRUCT(FDoGGradientPassPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)

		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, ViewPort)

		SHADER_PARAMETER(float, SigmaE)
		SHADER_PARAMETER(float, SigmaP)
		SHADER_PARAMETER(float, Tau)

		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, InBilateralTexture)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, InTangentFlowMapTexture)

		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()
};

IMPLEMENT_GLOBAL_SHADER(FDoGGradientPassPS, "/NPRTools/DoGGradient.usf", "DoGGradientPS", SF_Pixel);


class FDoGFlowPassPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FDoGFlowPassPS);
	SHADER_USE_PARAMETER_STRUCT(FDoGFlowPassPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)

		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, ViewPort)

		SHADER_PARAMETER(float, SigmaM)

		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, InDoGGradientTexture)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, InTangentFlowMapTexture)

		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()
};

IMPLEMENT_GLOBAL_SHADER(FDoGFlowPassPS, "/NPRTools/DoGFlow.usf", "DoGFlowPS", SF_Pixel);


class FQuantizePassPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FQuantizePassPS);
	SHADER_USE_PARAMETER_STRUCT(FQuantizePassPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)

		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, ViewPort)

		SHADER_PARAMETER(int, NumBins)
		SHADER_PARAMETER(float, Phi)

		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, InBilateralTexture)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, InDoGFlowTexture)

		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()
};

IMPLEMENT_GLOBAL_SHADER(FQuantizePassPS, "/NPRTools/Quantize.usf", "QuantizePS", SF_Pixel);


FNPRToolsViewExtension::FNPRToolsViewExtension(const FAutoRegister& AutoRegister)
	: FSceneViewExtensionBase(AutoRegister)
{
}


void FNPRToolsViewExtension::PrePostProcessPass_RenderThread(
	FRDGBuilder& GraphBuilder, 
	const FSceneView& View, 
	const FPostProcessingInputs& Inputs)
{
	if (!NPRTools::IsEnabled())
	{
		return;
	}

	// TODO: Expose these to be controlled by users
	constexpr float SigmaD1 = 3.0f;
	constexpr float SigmaR1 = 0.425f;
	constexpr float SigmaD2 = 0.34f;
	constexpr float SigmaR2 = 3.3f;

	constexpr float SigmaE = 1.0f;
	constexpr float SigmaP = 1.6f;
	constexpr float SigmaM = 3.0f;
	constexpr float Tau = 0.93999f;
	constexpr int32 NumBins = 16;
	constexpr float Phi = 3.4f;

	// Get the scene colour texture from the post process inputs
	Inputs.Validate();
	FRDGTextureRef SceneColorTexture = (*Inputs.SceneTextures)->SceneColorTexture;

	// Create a texture to hold the output of our Sobel filter
	// It should be the same format etc as the scene colour texture
	FRDGTextureDesc TextureDesc = SceneColorTexture->Desc;
	TextureDesc.ClearValue = FClearValueBinding(FLinearColor(0.0f, 0.0f, 0.0f));
	// TODO: All textures may not need to have 4 channels
	TextureDesc.Format = PF_FloatRGBA;

	FRDGTextureRef TangentFlowMapTexture = GraphBuilder.CreateTexture(TextureDesc, TEXT("NPRTools.TFM"));
	FRDGTextureRef ColorChangeTexture    = GraphBuilder.CreateTexture(TextureDesc, TEXT("NPRTools.ColorChange"));
	FRDGTextureRef TempPingTexture		 = GraphBuilder.CreateTexture(TextureDesc, TEXT("NPRTools.TempPing"));
	FRDGTextureRef TempPongTexture		 = GraphBuilder.CreateTexture(TextureDesc, TEXT("NPRTools.TempPong"));

	// We want to perform our postprocessing to the entire viewport
	FScreenPassTextureViewport ViewPort(TextureDesc.Extent);

	// Helper function to dispatch pass to avoid boilerplate
	auto AddPass = [&]<typename Shader, typename SetPassParametersLambdaType>(
		FRDGEventName&& PassName,
		FRDGTextureRef RenderTarget,
		SetPassParametersLambdaType && SetPassParametersLambda,
		typename Shader::FPermutationDomain Permutation = TShaderPermutationDomain()
		)
	{
		typename  Shader::FParameters* PassParameters = GraphBuilder.AllocParameters<typename Shader::FParameters>();
		PassParameters->View = View.ViewUniformBuffer;
		PassParameters->ViewPort = GetScreenPassTextureViewportParameters(ViewPort);

		SetPassParametersLambda(PassParameters);
		PassParameters->RenderTargets[0] = FRenderTargetBinding(RenderTarget, ERenderTargetLoadAction::ENoAction);

		const FGlobalShaderMap* ShaderMap = GetGlobalShaderMap(View.GetFeatureLevel());
		TShaderMapRef<Shader> PixelShader(ShaderMap, Permutation);

		AddDrawScreenPass(
			GraphBuilder,
			std::move(PassName),
			View,
			ViewPort,
			ViewPort,
			PixelShader,
			PassParameters
		);
	};

	// Calculate tangent flow map
	{
		
		AddPass.operator()<FSobelPassPS>(
			RDG_EVENT_NAME("NPRTools_SobelFilter"),
			TangentFlowMapTexture,
			[&](auto PassParameters)
			{
				PassParameters->SceneColorTexture = GraphBuilder.CreateSRV(SceneColorTexture);
			}
		);

		AddPass.operator()<FBlurEigenVerticalPassPS>(
			RDG_EVENT_NAME("NPRTools_EigenBlurVertical"),
			TempPingTexture,
			[&](auto PassParameters)
			{
				PassParameters->InTexture = GraphBuilder.CreateSRV(TangentFlowMapTexture);
			}
		);

		AddPass.operator()<FBlurEigenHorizontalPassPS>(
			RDG_EVENT_NAME("NPRTools_EigenBlurHorizontal"),
			TangentFlowMapTexture,
			[&](auto PassParameters)
			{
				PassParameters->InTexture = GraphBuilder.CreateSRV(TempPingTexture);
			}
		);
	}

	// Perform bilateral filtering
	{
		AddPass.operator()<FConvertYCCPassPS>(
			RDG_EVENT_NAME("NPRTools_ConvertYCC"),
			TempPongTexture,
			[&](auto PassParameters)
			{
				PassParameters->SceneColorTexture = GraphBuilder.CreateSRV(SceneColorTexture);
			}
		);

		FBilateralPassPS::FPermutationDomain Permutation;
		Permutation.Set<FBilateralPassPS::FBilateralDirectionTangent>(true);

		AddPass.operator()<FBilateralPassPS>(
			RDG_EVENT_NAME("NPRTools_BilateralTangent"),
			TempPingTexture,
			[&](FBilateralPassPS::FParameters* PassParameters)
			{
				PassParameters->SigmaD = SigmaD1;
				PassParameters->SigmaR = SigmaR1;

				PassParameters->InSceneColorYCCTexture = GraphBuilder.CreateSRV(TempPongTexture);
				PassParameters->InTangentFlowMapTexture = GraphBuilder.CreateSRV(TangentFlowMapTexture);
			},
			Permutation
		);

		Permutation.Set<FBilateralPassPS::FBilateralDirectionTangent>(false);

		AddPass.operator()<FBilateralPassPS>(
			RDG_EVENT_NAME("NPRTools_BilateralGradient"),
			ColorChangeTexture,
			[&](FBilateralPassPS::FParameters* PassParameters)
			{
				PassParameters->SigmaD = SigmaD2;
				PassParameters->SigmaR = SigmaR2;

				PassParameters->InSceneColorYCCTexture = GraphBuilder.CreateSRV(TempPingTexture);
				PassParameters->InTangentFlowMapTexture = GraphBuilder.CreateSRV(TangentFlowMapTexture);
			},
			Permutation
		);
	}

	// Perform Difference of Gaussians
	{
		AddPass.operator()<FDoGGradientPassPS>(
			RDG_EVENT_NAME("NPRTools_DoGGradient"),
			TempPingTexture,
			[&](auto PassParameters)
			{
				PassParameters->SigmaE = SigmaE;
				PassParameters->SigmaP = SigmaP;
				PassParameters->Tau = Tau;

				PassParameters->InBilateralTexture = GraphBuilder.CreateSRV(ColorChangeTexture);
				PassParameters->InTangentFlowMapTexture = GraphBuilder.CreateSRV(TangentFlowMapTexture);
			}
		);

		AddPass.operator()<FDoGFlowPassPS>(
			RDG_EVENT_NAME("NPRTools_DoGFlow"),
			TempPongTexture,
			[&](auto PassParameters)
			{
				PassParameters->SigmaM = SigmaM;

				PassParameters->InDoGGradientTexture = GraphBuilder.CreateSRV(TempPingTexture);
				PassParameters->InTangentFlowMapTexture = GraphBuilder.CreateSRV(TangentFlowMapTexture);
			}
		);
	}

	// Perform Quantization
	AddPass.operator()<FQuantizePassPS>(
		RDG_EVENT_NAME("NPRTools_Quantization"),
		SceneColorTexture,
		[&](auto PassParameters)
		{
			PassParameters->NumBins = NumBins;
			PassParameters->Phi = Phi;

			PassParameters->InBilateralTexture = GraphBuilder.CreateSRV(ColorChangeTexture);
			PassParameters->InDoGFlowTexture = GraphBuilder.CreateSRV(TempPongTexture);
		}
	);
}
 