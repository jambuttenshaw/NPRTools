#include "NPRPipeline.h"

#include "ScreenPass.h"
#include "RenderGraphBuilder.h"

#include "NPRTools.h"
#include "NPRToolsParametersProxy.h"
#include "NPRToolsHistory.h"
#include "NPRShaders.h"


static TAutoConsoleVariable<bool> CVarNPRToolsEnable(
	TEXT("npr.Enable"),
	true,
	TEXT("Enables NPR pipeline (Default = true)"),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<bool> CVarNPRToolsEnableForSceneCaptures(
	TEXT("npr.EnableForSceneCaptures"),
	false,
	TEXT("Enables NPR pipeline for scene capture components (this has found to be unstable) (Default = false)"),
	ECVF_RenderThreadSafe
);


DECLARE_GPU_STAT_NAMED(NPRToolsStat, TEXT("NPRTools"));

namespace NPRTools
{
	bool IsEnabled()
	{
		check(IsInGameThread());
		return CVarNPRToolsEnable.GetValueOnGameThread();
	}
	bool IsEnabled_RenderThread()
	{
		check(IsInRenderingThread());
		return CVarNPRToolsEnable.GetValueOnRenderThread();
	}

	bool IsEnabledForSceneCaptures()
	{
		check(IsInGameThread());
		return CVarNPRToolsEnableForSceneCaptures.GetValueOnGameThread();
	}
	bool IsEnabledForSceneCaptures_RenderThread()
	{
		check(IsInRenderingThread());
		return CVarNPRToolsEnableForSceneCaptures.GetValueOnRenderThread();
	}
}


bool NPRTools::ExecuteNPRPipeline(
	FRDGBuilder& GraphBuilder,
	const FNPRToolsParametersProxy& NPRParameters,
	FRDGTextureRef InColorTexture,
	FRDGTextureRef OutColorTexture, 
	FNPRToolsHistory* History
	)
{
	check(IsInRenderingThread());
	check(InColorTexture && OutColorTexture);
	
	if (!NPRParameters.bEnable)
		return false;

	if (!(NPRParameters.bCompositeColor || NPRParameters.bCompositeEdges))
		return false;

	RDG_EVENT_SCOPE_STAT(GraphBuilder, NPRToolsStat, "NPRTools");
	RDG_GPU_STAT_SCOPE(GraphBuilder, NPRToolsStat);
	SCOPED_NAMED_EVENT(NPRTools, FColor::Purple);

	bool bHistoryExists = History && History->IsValid();
	bool bSmoothTangents = NPRParameters.bSmoothTangents && bHistoryExists;

	FRDGTextureRef PrevTangentFlowMapTexture;
	if (bSmoothTangents)
	{
		PrevTangentFlowMapTexture = GraphBuilder.RegisterExternalTexture(History->PreviousTangentFlowMapTexture);
	}

	// Create a texture to hold the output of our Sobel filter
	// It should be the same format etc as the scene colour texture
	FRDGTextureDesc TextureDesc = InColorTexture->Desc;
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
	FScreenPassViewInfo ViewInfo(GMaxRHIFeatureLevel);

	// Helper function to dispatch pass to avoid boilerplate
	auto AddPass = [&]<typename Shader, typename SetPassParametersLambdaType>(
		FRDGEventName&& PassName,
		FRDGTextureRef RenderTarget,
		SetPassParametersLambdaType&& SetPassParametersLambda,
		typename Shader::FPermutationDomain Permutation = TShaderPermutationDomain())
	{
		typename Shader::FParameters* PassParameters = GraphBuilder.AllocParameters<typename Shader::FParameters>();
		PassParameters->ViewPort = GetScreenPassTextureViewportParameters(ViewPort);
		PassParameters->sampler0 = TStaticSamplerState<>::GetRHI(); // Point clamped sampler

		SetPassParametersLambda(PassParameters);
		PassParameters->RenderTargets[0] = FRenderTargetBinding(RenderTarget, ERenderTargetLoadAction::ENoAction);

		const FGlobalShaderMap* ShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
		TShaderMapRef<Shader> PixelShader(ShaderMap, Permutation);

		AddDrawScreenPass(
			GraphBuilder,
			std::move(PassName),
			ViewInfo,
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
				PassParameters->SceneColorTexture = GraphBuilder.CreateSRV(InColorTexture);
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

		FBlurEigenHorizontalPassPS::FPermutationDomain Permutation;
		Permutation.Set<FBlurEigenHorizontalPassPS::FSmoothTangents>(bSmoothTangents);
		AddPass.operator()<FBlurEigenHorizontalPassPS>(
			RDG_EVENT_NAME("EigenBlur(Horizontal)"),
			TangentFlowMapTexture,
			[&](auto PassParameters)
			{
				if (bSmoothTangents)
				{
					PassParameters->SmoothingAmount = NPRParameters.SmoothingAmount;
					PassParameters->InPrevTangentFlowMap = GraphBuilder.CreateSRV(PrevTangentFlowMapTexture);
				}
				PassParameters->InTexture = GraphBuilder.CreateSRV(TempPingTexture);
			},
			Permutation
		);
	}

	AddPass.operator()<FConvertYCCPassPS>(
		RDG_EVENT_NAME("ConvertYCC"),
		TempPongTexture,
		[&](auto PassParameters)
		{
			PassParameters->SceneColorTexture = GraphBuilder.CreateSRV(InColorTexture);
		}
	);

	// Copy ensures that the ping-ponging will be correct for multiple bilateral filters
	AddCopyTexturePass(
		GraphBuilder,
		TempPongTexture,
		ColorChangeTexture
	);

	// Perform bilateral filtering
	for (int32 i = 0; i < NPRParameters.NumBilateralFilterPasses; i++)
	{
		FBilateralPassPS::FPermutationDomain Permutation;
		Permutation.Set<FBilateralPassPS::FBilateralDirectionTangent>(true);

		AddPass.operator()<FBilateralPassPS>(
			RDG_EVENT_NAME("Bilateral(Tangent)"),
			TempPingTexture,
			[&](auto PassParameters)
			{
				PassParameters->SigmaD = NPRParameters.SigmaD1;
				PassParameters->SigmaR = NPRParameters.SigmaR1;

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
				PassParameters->SigmaD = NPRParameters.SigmaD2;
				PassParameters->SigmaR = NPRParameters.SigmaR2;

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
				PassParameters->SigmaE = NPRParameters.SigmaE;				 // Following convention in paper where SigmaE' = SigmaE * K, 
				PassParameters->SigmaP = NPRParameters.SigmaE * NPRParameters.K; // and allowing users to modify SigmaE and K
				PassParameters->Tau = NPRParameters.Tau;

				PassParameters->InBilateralTexture = GraphBuilder.CreateSRV(ColorChangeTexture);
				PassParameters->InTangentFlowMapTexture = GraphBuilder.CreateSRV(TangentFlowMapTexture);
			}
		);

		FDoGFlowPassPS::FPermutationDomain Permutation;
		Permutation.Set<FDoGFlowPassPS::FThresholdingMethod>(static_cast<int>(NPRParameters.ThresholdingMethod));
		AddPass.operator()<FDoGFlowPassPS>(
			RDG_EVENT_NAME("DoG(Flow)"),
			TempPongTexture,
			[&](auto PassParameters)
			{
				PassParameters->SigmaM = NPRParameters.SigmaM;
				PassParameters->Epsilon = NPRParameters.Epsilon;
				PassParameters->Phi = NPRParameters.PhiEdge;

				PassParameters->InDoGGradientTexture = GraphBuilder.CreateSRV(TempPingTexture);
				PassParameters->InTangentFlowMapTexture = GraphBuilder.CreateSRV(TangentFlowMapTexture);
			},
			Permutation
		);
	}

	if (NPRParameters.bEnableQuantization && NPRParameters.bCompositeColor)
	{
		if (NPRParameters.bUseKuwahara)
		{
			// Run Kuwahara pass
			AddPass.operator()<FKuwaharaPassPS>(
				RDG_EVENT_NAME("Kuwahara"),
				TempPingTexture,
				[&](auto PassParameters)
				{
					PassParameters->KernelSize = NPRParameters.KuwaharaKernelSize;
					PassParameters->Hardness = NPRParameters.KuwaharaHardness;
					PassParameters->Sharpness = NPRParameters.KuwaharaSharpness;
					PassParameters->Alpha = NPRParameters.KuwaharaAlpha;
					PassParameters->ZeroCrossing = NPRParameters.KuwaharaZeroCrossing;
					PassParameters->Zeta = NPRParameters.KuwaharaZeta;

					PassParameters->SceneColorTexture = GraphBuilder.CreateSRV(InColorTexture);
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
					PassParameters->NumBins = NPRParameters.NumBins;
					PassParameters->Phi = NPRParameters.PhiColor;

					PassParameters->InBilateralTexture = GraphBuilder.CreateSRV(ColorChangeTexture);
				}
			);
		}
	}
	else
	{
		// Copy scene colour texture as that has not been converted to YCC
		AddCopyTexturePass(GraphBuilder, InColorTexture, TempPingTexture);
	}

	if (NPRParameters.bCompositeColor && NPRParameters.bCompositeEdges)
	{
		// Combine edges and quantized color
		AddPass.operator()<FCombineEdgesPassPS>(
			RDG_EVENT_NAME("CombineEdges"),
			OutColorTexture,
			[&](auto PassParameters)
			{
				PassParameters->InColorTexture = GraphBuilder.CreateSRV(TempPingTexture);
				PassParameters->InEdgesTexture = GraphBuilder.CreateSRV(TempPongTexture);
			}
		);
	}
	else if (NPRParameters.bCompositeColor)
	{
		AddCopyTexturePass(GraphBuilder, TempPingTexture, OutColorTexture);
	}
	else if (NPRParameters.bCompositeEdges)
	{
		AddCopyTexturePass(GraphBuilder, TempPongTexture, OutColorTexture);
	}
	else
	{
		UE_LOG(LogNPRTools, Error, TEXT("Parameter configuration error: Invalid composition mode!"));
	}

	// Save tangent flow map for next frame for temporal smoothing
	if (bHistoryExists)
	{
		GraphBuilder.QueueTextureExtraction(TangentFlowMapTexture, &History->PreviousTangentFlowMapTexture);
	}

	return true;
}
