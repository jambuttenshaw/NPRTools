#include "NPRToolsViewExtension.h"

#include "NPRTools.h"
#include "NPRToolsWorldSubsystem.h"

#include "ShaderParameterStruct.h"
#include "ScreenPass.h"

#include "PostProcess/PostProcessInputs.h"
#include "RenderTargetPool.h"
#include "SceneTextures.h"


static TAutoConsoleVariable<bool> CVarNPRToolsEnable(
	TEXT("npr.Enable"),
	true,
	TEXT("Enables NPR pipeline (Default = true)"),
	ECVF_RenderThreadSafe
);


namespace NPRTools
{
	// These are all functions for access on RENDER THREAD
	static bool IsEnabled()
	{
		return CVarNPRToolsEnable.GetValueOnRenderThread();
	}
}

DECLARE_GPU_STAT_NAMED(NPRToolsStat, TEXT("NPRTools"));


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

IMPLEMENT_GLOBAL_SHADER(FBlurEigenVerticalPassPS, "/NPRTools/EigenBlur.usf", "BlurEigenVerticalPS", SF_Pixel);


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

IMPLEMENT_GLOBAL_SHADER(FBlurEigenHorizontalPassPS, "/NPRTools/EigenBlur.usf", "BlurEigenHorizontalPS", SF_Pixel);


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

	class FThresholdingMethod : SHADER_PERMUTATION_INT("THRESHOLDING_METHOD", 2);
	using FPermutationDomain = TShaderPermutationDomain<FThresholdingMethod>;

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)

		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, ViewPort)

		SHADER_PARAMETER(float, SigmaM)
		SHADER_PARAMETER(float, Epsilon)
		SHADER_PARAMETER(float, Phi)

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

		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()
};

IMPLEMENT_GLOBAL_SHADER(FQuantizePassPS, "/NPRTools/Quantize.usf", "QuantizePS", SF_Pixel);


class FKuwaharaPassPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FKuwaharaPassPS);
	SHADER_USE_PARAMETER_STRUCT(FKuwaharaPassPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)

		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, ViewPort)

		SHADER_PARAMETER(int32, KernelSize)
		SHADER_PARAMETER(float, Hardness)
		SHADER_PARAMETER(float, Sharpness)
		SHADER_PARAMETER(float, Alpha)
		SHADER_PARAMETER(float, ZeroCrossing)
		SHADER_PARAMETER(float, Zeta)

		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, SceneColorTexture)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, GaussianLUTTexture)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, TangentFlowMapTexture)

		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()
};

IMPLEMENT_GLOBAL_SHADER(FKuwaharaPassPS, "/NPRTools/Kuwahara.usf", "KuwaharaPS", SF_Pixel);


class FCombineEdgesPassPS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FCombineEdgesPassPS);
	SHADER_USE_PARAMETER_STRUCT(FCombineEdgesPassPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)

		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, ViewPort)

		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, InColorTexture)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, InEdgesTexture)

		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()
};

IMPLEMENT_GLOBAL_SHADER(FCombineEdgesPassPS, "/NPRTools/CombineEdges.usf", "CombineEdgesPS", SF_Pixel);


FNPRToolsViewExtension::FNPRToolsViewExtension(const FAutoRegister& AutoRegister, UNPRToolsWorldSubsystem* InWorldSubsystem)
	: FSceneViewExtensionBase(AutoRegister)
	, WorldSubsystem(InWorldSubsystem)
{
	GaussianLUT = LoadObject<UTexture2D>(nullptr, TEXT("/Script/Engine.Texture2D'/NPRTools/Textures/T_KDE.T_KDE'"));
}

void FNPRToolsViewExtension::BeginRenderViewFamily(FSceneViewFamily& InViewFamily)
{
	if (IsValid(WorldSubsystem))
		WorldSubsystem->TransferState();
}

bool FNPRToolsViewExtension::IsActiveThisFrame_Internal(const FSceneViewExtensionContext& Context) const
{
	return CVarNPRToolsEnable.GetValueOnGameThread() && IsValid(WorldSubsystem);
}


void FNPRToolsViewExtension::PrePostProcessPass_RenderThread(
	FRDGBuilder& GraphBuilder, 
	const FSceneView& View, 
	const FPostProcessingInputs& Inputs)
{
	if (!IsValid(WorldSubsystem) || !WorldSubsystem->ParametersProxy.IsValid())
		return;

	FNPRToolsParametersProxy* Parameters = WorldSubsystem->ParametersProxy.Get();
	if (!Parameters->bEnable)
		return;

	RDG_EVENT_SCOPE_STAT(GraphBuilder, NPRToolsStat, "NPRTools");
	RDG_GPU_STAT_SCOPE(GraphBuilder, NPRToolsStat);
	SCOPED_NAMED_EVENT(NPRTools, FColor::Purple);

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

	AddClearRenderTargetPass(GraphBuilder, TangentFlowMapTexture);
	AddClearRenderTargetPass(GraphBuilder, ColorChangeTexture);
	AddClearRenderTargetPass(GraphBuilder, TempPingTexture);
	AddClearRenderTargetPass(GraphBuilder, TempPongTexture);

	// We want to perform our postprocessing to the entire viewport
	FScreenPassTextureViewport ViewPort(TextureDesc.Extent);

	// Helper function to dispatch pass to avoid boilerplate
	auto AddPass = [&]<typename Shader, typename SetPassParametersLambdaType>(
		FRDGEventName&& PassName,
		FRDGTextureRef RenderTarget,
		SetPassParametersLambdaType&& SetPassParametersLambda,
		typename Shader::FPermutationDomain Permutation = TShaderPermutationDomain())
	{
		typename Shader::FParameters* PassParameters = GraphBuilder.AllocParameters<typename Shader::FParameters>();
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
			RDG_EVENT_NAME("Sobel"),
			TangentFlowMapTexture,
			[&](auto PassParameters)
			{
				PassParameters->SceneColorTexture = GraphBuilder.CreateSRV(SceneColorTexture);
			}
		);

		AddPass.operator()<FBlurEigenVerticalPassPS>(
			RDG_EVENT_NAME("EigenBlur(Vertical)"),
			TempPingTexture,
			[&](auto PassParameters)
			{
				PassParameters->InTexture = GraphBuilder.CreateSRV(TangentFlowMapTexture);
			}
		);

		AddPass.operator()<FBlurEigenHorizontalPassPS>(
			RDG_EVENT_NAME("EigenBlur(Horizontal)"),
			TangentFlowMapTexture,
			[&](auto PassParameters)
			{
				PassParameters->InTexture = GraphBuilder.CreateSRV(TempPingTexture);
			}
		);
	}

	AddPass.operator()<FConvertYCCPassPS>(
		RDG_EVENT_NAME("ConvertYCC"),
		TempPongTexture,
		[&](auto PassParameters)
		{
			PassParameters->SceneColorTexture = GraphBuilder.CreateSRV(SceneColorTexture);
		}
	);

	// Copy ensures that the ping-ponging will be correct for multiple bilateral filters
	AddCopyTexturePass(
		GraphBuilder,
		TempPongTexture,
		ColorChangeTexture
	);

	// Perform bilateral filtering
	for (int32 i = 0; i < Parameters->NumBilateralFilterPasses; i++)
	{
		FBilateralPassPS::FPermutationDomain Permutation;
		Permutation.Set<FBilateralPassPS::FBilateralDirectionTangent>(true);

		AddPass.operator()<FBilateralPassPS>(
			RDG_EVENT_NAME("Bilateral(Tangent)"),
			TempPingTexture,
			[&](auto PassParameters)
			{
				PassParameters->SigmaD = Parameters->SigmaD1;
				PassParameters->SigmaR = Parameters->SigmaR1;

				PassParameters->InSceneColorYCCTexture = GraphBuilder.CreateSRV(ColorChangeTexture);
				PassParameters->InTangentFlowMapTexture = GraphBuilder.CreateSRV(TangentFlowMapTexture);
			},
			Permutation
		);

		Permutation.Set<FBilateralPassPS::FBilateralDirectionTangent>(false);

		AddPass.operator()<FBilateralPassPS>(
			RDG_EVENT_NAME("Bilateral(Gradient)"),
			ColorChangeTexture,
			[&](auto PassParameters)
			{
				PassParameters->SigmaD = Parameters->SigmaD2;
				PassParameters->SigmaR = Parameters->SigmaR2;

				PassParameters->InSceneColorYCCTexture = GraphBuilder.CreateSRV(TempPingTexture);
				PassParameters->InTangentFlowMapTexture = GraphBuilder.CreateSRV(TangentFlowMapTexture);
			},
			Permutation
		);
	}

	// Perform Difference of Gaussians
	{
		AddPass.operator()<FDoGGradientPassPS>(
			RDG_EVENT_NAME("DoG(Gradient)"),
			TempPingTexture,
			[&](auto PassParameters)
			{
				PassParameters->SigmaE = Parameters->SigmaE;				 // Following convention in paper where SigmaE' = SigmaE * K, 
				PassParameters->SigmaP = Parameters->SigmaE * Parameters->K; // and allowing users to modify SigmaE and K
				PassParameters->Tau = Parameters->Tau;

				PassParameters->InBilateralTexture = GraphBuilder.CreateSRV(ColorChangeTexture);
				PassParameters->InTangentFlowMapTexture = GraphBuilder.CreateSRV(TangentFlowMapTexture);
			}
		);

		FDoGFlowPassPS::FPermutationDomain Permutation;
		Permutation.Set<FDoGFlowPassPS::FThresholdingMethod>(static_cast<int>(Parameters->ThresholdingMethod));
		AddPass.operator()<FDoGFlowPassPS>(
			RDG_EVENT_NAME("DoG(Flow)"),
			TempPongTexture,
			[&](auto PassParameters)
			{
				PassParameters->SigmaM = Parameters->SigmaM;
				PassParameters->Epsilon = Parameters->Epsilon;
				PassParameters->Phi = Parameters->PhiEdge;

				PassParameters->InDoGGradientTexture = GraphBuilder.CreateSRV(TempPingTexture);
				PassParameters->InTangentFlowMapTexture = GraphBuilder.CreateSRV(TangentFlowMapTexture);
			},
			Permutation
		);
	}

	if (Parameters->bEnableQuantization && Parameters->CompositionMode != ENPRToolsCompositionMode::EdgesOnly)
	{
		if (Parameters->bUseKuwahara)
		{
			// Register Gaussian kernel LUT into RDG
			FRDGTextureRef LUT_RDG = GSystemTextures.GetBlackDummy(GraphBuilder);
			if (GaussianLUT.IsValid(false, true) && GaussianLUT->GetResource())
			{
				FTextureRHIRef LUT_RHI = GaussianLUT->GetResource()->TextureRHI;

				// Register external texture to RDG;
				FPooledRenderTargetDesc desc = FPooledRenderTargetDesc::Create2DDesc(
					LUT_RHI->GetSizeXY(),
					LUT_RHI->GetFormat(),
					FClearValueBinding::Black,
					TexCreate_None,
					TexCreate_ShaderResource,
					false,
					LUT_RHI->GetNumMips());

				FSceneRenderTargetItem renderTargetItem;
				renderTargetItem.TargetableTexture = LUT_RHI;
				renderTargetItem.ShaderResourceTexture = LUT_RHI;

				TRefCountPtr<IPooledRenderTarget> PhaseFunctionLUT_RT;
				GRenderTargetPool.CreateUntrackedElement(desc, PhaseFunctionLUT_RT, renderTargetItem);
				LUT_RDG = GraphBuilder.RegisterExternalTexture(PhaseFunctionLUT_RT);
			}

			// Run Kuwahara pass
			AddPass.operator()<FKuwaharaPassPS>(
				RDG_EVENT_NAME("Kuwahara"),
				TempPingTexture,
				[&](auto PassParameters)
				{
					PassParameters->KernelSize = Parameters->KuwaharaKernelSize;
					PassParameters->Hardness = Parameters->KuwaharaHardness;
					PassParameters->Sharpness = Parameters->KuwaharaSharpness;
					PassParameters->Alpha = Parameters->KuwaharaAlpha;
					PassParameters->ZeroCrossing = Parameters->KuwaharaZeroCrossing;
					PassParameters->Zeta = Parameters->KuwaharaZeta;

					PassParameters->SceneColorTexture = GraphBuilder.CreateSRV(SceneColorTexture);
					PassParameters->GaussianLUTTexture = GraphBuilder.CreateSRV(LUT_RDG);
					PassParameters->TangentFlowMapTexture = GraphBuilder.CreateSRV(TangentFlowMapTexture);
				}
			);
		}
		else
		{
			// Perform Quantization
			AddPass.operator()<FQuantizePassPS>(
				RDG_EVENT_NAME("Quantization"),
				TempPingTexture,
				[&](auto PassParameters)
				{
					PassParameters->NumBins = Parameters->NumBins;
					PassParameters->Phi = Parameters->PhiColor;

					PassParameters->InBilateralTexture = GraphBuilder.CreateSRV(ColorChangeTexture);
				}
			);
		}
	}
	else
	{
		// Copy scene colour texture as that has not been converted to YCC
		AddCopyTexturePass(GraphBuilder, SceneColorTexture, TempPingTexture);
	}

	if (Parameters->CompositionMode == ENPRToolsCompositionMode::ColourOnly)
	{
		AddCopyTexturePass(GraphBuilder, TempPingTexture, SceneColorTexture);
	}
	else if (Parameters->CompositionMode == ENPRToolsCompositionMode::EdgesOnly)
	{
		AddCopyTexturePass(GraphBuilder, TempPongTexture, SceneColorTexture);
	}
	else if (Parameters->CompositionMode == ENPRToolsCompositionMode::ColourAndEdges)
	{
		// Combine edges and quantized color
		AddPass.operator()<FCombineEdgesPassPS>(
			RDG_EVENT_NAME("CombineEdges"),
			SceneColorTexture,
			[&](auto PassParameters)
			{
				PassParameters->InColorTexture = GraphBuilder.CreateSRV(TempPingTexture);
				PassParameters->InEdgesTexture = GraphBuilder.CreateSRV(TempPongTexture);
			}
		);
	}
	else
	{
		UE_LOG(LogNPRTools, Error, TEXT("Invalid composition mode! Composition mode = %d"), static_cast<uint32>(Parameters->CompositionMode));
	}
 }