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


template <typename Shader>
void AddNPRPass(
	FRDGBuilder& GraphBuilder,
	FRDGEventName&& PassName,
	FRDGTextureRef RenderTarget,
	std::function<void(typename Shader::FParameters*)>&& SetPassParametersLambda,
	typename Shader::FPermutationDomain Permutation = TShaderPermutationDomain(),
	FIntRect OutRect = FIntRect(),
	FIntRect InRect = FIntRect()
)
{
	FScreenPassTextureViewport OutViewPort = OutRect.IsEmpty() ?
												FScreenPassTextureViewport{ RenderTarget->Desc.Extent } :
												FScreenPassTextureViewport{ RenderTarget->Desc.Extent, OutRect };
	FScreenPassTextureViewport InViewPort = InRect.IsEmpty() ?
												FScreenPassTextureViewport{ RenderTarget->Desc.Extent } :
												FScreenPassTextureViewport{ RenderTarget->Desc.Extent, InRect };

	typename Shader::FParameters* PassParameters = GraphBuilder.AllocParameters<typename Shader::FParameters>();
	PassParameters->OutViewPort = GetScreenPassTextureViewportParameters(OutViewPort);
	PassParameters->InViewPort = GetScreenPassTextureViewportParameters(InViewPort);
	PassParameters->sampler0 = TStaticSamplerState<>::GetRHI(); // Point clamped sampler

	SetPassParametersLambda(PassParameters);
	PassParameters->RenderTargets[0] = FRenderTargetBinding(RenderTarget, ERenderTargetLoadAction::ENoAction);

	const FGlobalShaderMap* ShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
	TShaderMapRef<Shader> PixelShader(ShaderMap, Permutation);

	AddDrawScreenPass(
		GraphBuilder,
		std::move(PassName),
		FScreenPassViewInfo{GMaxRHIFeatureLevel},
		OutViewPort,
		InViewPort,
		PixelShader,
		PassParameters
	);
}


// Ensures compatible texture descriptions for all textures in the pipeline
FRDGTextureRef CreateTextureFrom(FRDGBuilder& GraphBuilder, FRDGTextureRef InTex, const TCHAR* Name, float ScaleFactor = 1.0f, EPixelFormat Format = PF_FloatRGBA)
{
	FRDGTextureDesc Desc = InTex->Desc;
	Desc.ClearValue = FClearValueBinding(FLinearColor(0.0f, 0.0f, 0.0f));
	Desc.Format = Format;
	Desc.Extent.X = static_cast<int>(static_cast<float>(Desc.Extent.X) * ScaleFactor);
	Desc.Extent.Y = static_cast<int>(static_cast<float>(Desc.Extent.Y) * ScaleFactor);
	return GraphBuilder.CreateTexture(Desc, Name);
}


FRDGTextureRef GaussianBlur(
	FRDGBuilder& GraphBuilder,
	FRDGTextureRef InTexture,
	float Sigma
)
{
	FRDGTextureRef Temp = CreateTextureFrom(GraphBuilder, InTexture, TEXT("NPRTools.GaussianBlur.Temp"));
	FRDGTextureRef Output = CreateTextureFrom(GraphBuilder, InTexture, TEXT("NPRTools.GaussianBlur.Output"));

	FSeparableGaussianBlurPS::FPermutationDomain Permutation;
	Permutation.Set<FSeparableGaussianBlurPS::FSeparableDirectionHorizontal>(true);

	AddNPRPass<FSeparableGaussianBlurPS>(
		GraphBuilder,
		RDG_EVENT_NAME("GaussianBlur_Horizontal"),
		Temp,
		[&](auto PassParameters)
		{
			PassParameters->InTexture = GraphBuilder.CreateSRV(InTexture);
			PassParameters->Sigma = Sigma;
		},
		Permutation
	);

	Permutation.Set<FSeparableGaussianBlurPS::FSeparableDirectionHorizontal>(false);

	AddNPRPass<FSeparableGaussianBlurPS>(
		GraphBuilder,
		RDG_EVENT_NAME("GaussianBlur_Vertical"),
		Output,
		[&](auto PassParameters)
		{
			PassParameters->InTexture = GraphBuilder.CreateSRV(Temp);
			PassParameters->Sigma = Sigma;
		},
		Permutation
	);

	return Output;
}


FRDGTextureRef CreateTangentFlowMap(
	FRDGBuilder& GraphBuilder,
	const FNPRToolsParametersProxy& NPRParameters,
	bool bSmoothTangents,
	FRDGTextureRef InColorTexture,
	FRDGTextureRef InPrevTangentFlowMapTexture
)
{
	FRDGTextureRef TangentFlowMapTexture = CreateTextureFrom(GraphBuilder, InColorTexture, TEXT("NPRTools.TFM"));
	FRDGTextureRef TempPingTexture = CreateTextureFrom(GraphBuilder, InColorTexture, TEXT("NPRTools.TFM.Temp"));

	AddNPRPass<FSobelPassPS>(
		GraphBuilder,
		RDG_EVENT_NAME("Sobel"),
		TangentFlowMapTexture,
		[&](auto PassParameters)
		{
			PassParameters->SceneColorTexture = GraphBuilder.CreateSRV(InColorTexture);
		}
	);

	AddNPRPass<FBlurEigenVerticalPassPS>(
		GraphBuilder,
		RDG_EVENT_NAME("EigenBlur(Vertical)"),
		TempPingTexture,
		[&](auto PassParameters)
		{
			PassParameters->InTexture = GraphBuilder.CreateSRV(TangentFlowMapTexture);
		}
	);

	FBlurEigenHorizontalPassPS::FPermutationDomain Permutation;
	Permutation.Set<FBlurEigenHorizontalPassPS::FSmoothTangents>(bSmoothTangents);
	AddNPRPass<FBlurEigenHorizontalPassPS>(
		GraphBuilder,
		RDG_EVENT_NAME("EigenBlur(Horizontal)"),
		TangentFlowMapTexture,
		[&](auto PassParameters)
		{
			if (bSmoothTangents)
			{
				check(InPrevTangentFlowMapTexture);
				PassParameters->SmoothingAmount = NPRParameters.SmoothingAmount;
				PassParameters->InPrevTangentFlowMap = GraphBuilder.CreateSRV(InPrevTangentFlowMapTexture);
			}
			PassParameters->InTexture = GraphBuilder.CreateSRV(TempPingTexture);
		},
		Permutation
	);

	return TangentFlowMapTexture;
}

FRDGTextureRef ConvertToYCC(
	FRDGBuilder& GraphBuilder,
	FRDGTextureRef InRGBTexture
)
{
	FRDGTextureRef YCCTexture = CreateTextureFrom(GraphBuilder, InRGBTexture, TEXT("NPRTools.YCC"));

	AddNPRPass<FConvertYCCPassPS>(
		GraphBuilder,
		RDG_EVENT_NAME("ConvertYCC"),
		YCCTexture,
		[&](auto PassParameters)
		{
			PassParameters->SceneColorTexture = GraphBuilder.CreateSRV(InRGBTexture);
		}
	);

	return YCCTexture;
}

FRDGTextureRef BilateralFilter(
	FRDGBuilder& GraphBuilder,
	const FNPRBilateralFilterParametersProxy& BilateralFilterParameters,
	FRDGTextureRef InTexture,
	FRDGTextureRef InTangentFlowMapTexture
)
{
	if (BilateralFilterParameters.NumPasses == 0)
	{
		return InTexture;
	}

	FRDGTextureRef OutTexture = CreateTextureFrom(GraphBuilder, InTexture, TEXT("NPRTools.Bilateral"));
	FRDGTextureRef TempTexture = CreateTextureFrom(GraphBuilder, InTexture, TEXT("NPRTools.Bilateral.Temp"));

	// Perform bilateral filtering
	for (int32 i = 0; i < BilateralFilterParameters.NumPasses; i++)
	{
		FBilateralPassPS::FPermutationDomain Permutation;
		Permutation.Set<FBilateralPassPS::FBilateralDirectionTangent>(true);

		AddNPRPass<FBilateralPassPS>(
			GraphBuilder,
			RDG_EVENT_NAME("Bilateral(Tangent)"),
			TempTexture,
			[&](auto PassParameters)
			{
				PassParameters->SigmaD = BilateralFilterParameters.SigmaD1;
				PassParameters->SigmaR = BilateralFilterParameters.SigmaR1;

				PassParameters->InSceneColorYCCTexture = GraphBuilder.CreateSRV(i == 0 ? InTexture : OutTexture);
				PassParameters->InTangentFlowMapTexture = GraphBuilder.CreateSRV(InTangentFlowMapTexture);
			},
			Permutation
		);

		Permutation.Set<FBilateralPassPS::FBilateralDirectionTangent>(false);

		AddNPRPass<FBilateralPassPS>(
			GraphBuilder,
			RDG_EVENT_NAME("Bilateral(Gradient)"),
			OutTexture,
			[&](auto PassParameters)
			{
				PassParameters->SigmaD = BilateralFilterParameters.SigmaD2;
				PassParameters->SigmaR = BilateralFilterParameters.SigmaR2;

				PassParameters->InSceneColorYCCTexture = GraphBuilder.CreateSRV(TempTexture);
				PassParameters->InTangentFlowMapTexture = GraphBuilder.CreateSRV(InTangentFlowMapTexture);
			},
			Permutation
		);
	}

	return OutTexture;
}

FRDGTextureRef DifferenceOfGaussians(
	FRDGBuilder& GraphBuilder,
	const FNPRDifferenceOfGaussiansParametersProxy& DoGParameters,
	FRDGTextureRef InBilateralTexture,
	FRDGTextureRef InTangentFlowMapTexture
)
{
	FRDGTextureRef OutTexture = CreateTextureFrom(GraphBuilder, InBilateralTexture, TEXT("NPRTools.DoG"));
	FRDGTextureRef TempTexture = CreateTextureFrom(GraphBuilder, InBilateralTexture, TEXT("NPRTools.DoG.Temp"));

	AddNPRPass<FDoGGradientPassPS>(
		GraphBuilder,
		RDG_EVENT_NAME("DoG(Gradient)"),
		TempTexture,
		[&](auto PassParameters)
		{
			PassParameters->SigmaE = DoGParameters.SigmaE;				 // Following convention in paper where SigmaE' = SigmaE * K, 
			PassParameters->SigmaP = DoGParameters.SigmaE * DoGParameters.K; // and allowing users to modify SigmaE and K
			PassParameters->Tau = DoGParameters.Tau;

			PassParameters->InBilateralTexture = GraphBuilder.CreateSRV(InBilateralTexture);
			PassParameters->InTangentFlowMapTexture = GraphBuilder.CreateSRV(InTangentFlowMapTexture);
		}
	);

	FDoGFlowPassPS::FPermutationDomain Permutation;
	Permutation.Set<FDoGFlowPassPS::FThresholdingMethod>(static_cast<int>(DoGParameters.ThresholdingMethod));
	AddNPRPass<FDoGFlowPassPS>(
		GraphBuilder,
		RDG_EVENT_NAME("DoG(Flow)"),
		OutTexture,
		[&](auto PassParameters)
		{
			PassParameters->SigmaM = DoGParameters.SigmaM;
			PassParameters->Epsilon = DoGParameters.Epsilon;
			PassParameters->Phi = DoGParameters.PhiEdge;

			PassParameters->InDoGGradientTexture = GraphBuilder.CreateSRV(TempTexture);
			PassParameters->InTangentFlowMapTexture = GraphBuilder.CreateSRV(InTangentFlowMapTexture);
		},
		Permutation
	);

	return OutTexture;
}

FRDGTextureRef QuantizeFilter(
	FRDGBuilder& GraphBuilder,
	const FNPRQuantizationParametersProxy& QuantizationParameters,
	FRDGTextureRef InYCCTexture
)
{
	FRDGTextureRef OutTexture = CreateTextureFrom(GraphBuilder, InYCCTexture, TEXT("NPRTools.Quantize"));

	AddNPRPass<FQuantizePassPS>(
		GraphBuilder,
		RDG_EVENT_NAME("Quantization"),
		OutTexture,
		[&](auto PassParameters)
		{
			PassParameters->NumBins = QuantizationParameters.NumBins;
			PassParameters->Phi = QuantizationParameters.PhiColor;

			PassParameters->InBilateralTexture = GraphBuilder.CreateSRV(InYCCTexture);
		}
	);

	return OutTexture;
}

FRDGTextureRef KuwaharaFilter(
	FRDGBuilder& GraphBuilder,
	const FNPRKuwaharaParametersProxy& KuwaharaParameters,
	FRDGTextureRef InColorTexture,
	FRDGTextureRef InTangentFlowMapTexture
)
{
	FRDGTextureRef OutTexture = CreateTextureFrom(GraphBuilder, InColorTexture, TEXT("NPRTools.Kuwahara"));

	AddNPRPass<FKuwaharaPassPS>(
		GraphBuilder,
		RDG_EVENT_NAME("Kuwahara"),
		OutTexture,
		[&](auto PassParameters)
		{
			PassParameters->KernelSize = KuwaharaParameters.KernelSize;
			PassParameters->Hardness = KuwaharaParameters.Hardness;
			PassParameters->Sharpness = KuwaharaParameters.Sharpness;
			PassParameters->Alpha = KuwaharaParameters.Alpha;
			PassParameters->ZeroCrossing = KuwaharaParameters.ZeroCrossing;
			PassParameters->Zeta = KuwaharaParameters.Zeta;

			PassParameters->SceneColorTexture = GraphBuilder.CreateSRV(InColorTexture);
			PassParameters->TangentFlowMapTexture = GraphBuilder.CreateSRV(InTangentFlowMapTexture);
		}
	);

	return OutTexture;
}

FRDGTextureRef OilPaintFilter(
	FRDGBuilder& GraphBuilder,
	const FNPROilPaintParametersProxy& OilPaintParameters,
	FRDGTextureRef InColorTexture
)
{
	FRDGTextureRef OutTexture = CreateTextureFrom(GraphBuilder, InColorTexture, TEXT("NPRTools.OilPaint"));
	FRDGTextureRef TempTexture = CreateTextureFrom(GraphBuilder, InColorTexture, TEXT("NPRTools.OilPaint.Temp"));

	AddNPRPass<FOilPaintStrokesPS>(
		GraphBuilder,
		RDG_EVENT_NAME("OilPaint(Strokes)"),
		TempTexture,
		[&](auto PassParameters)
		{
			PassParameters->BrushDetail = OilPaintParameters.BrushDetail;
			PassParameters->StrokeBend = OilPaintParameters.StrokeBend;
			PassParameters->BrushSize = OilPaintParameters.BrushSize;

			PassParameters->InColorTexture = GraphBuilder.CreateSRV(InColorTexture);
		}
	);

	if (!OilPaintParameters.bEnableReliefLighting)
	{
		return TempTexture;
	}
		
	AddNPRPass<FOilPaintReliefLightingPS>(
		GraphBuilder,
		RDG_EVENT_NAME("OilPaint(Relief)"),
		OutTexture,
		[&](auto PassParameters)
		{
			PassParameters->PaintSpec = OilPaintParameters.PaintSpecular;

			PassParameters->InColorTexture = GraphBuilder.CreateSRV(TempTexture);
		}
	);

	return OutTexture;
}

FRDGTextureRef PencilSketchFilter(
	FRDGBuilder& GraphBuilder,
	const FNPRPencilSketchParametersProxy& PencilSketchParameters,
	FRDGTextureRef InColorTexture
)
{
	FRDGTextureRef OutTexture = CreateTextureFrom(GraphBuilder, InColorTexture, TEXT("NPRTools.PencilSketch"));
	AddNPRPass<FPencilSketchPS>(
		GraphBuilder,
		RDG_EVENT_NAME("PencilSketch"),
		OutTexture,
		[&](auto PassParameters)
		{
			PassParameters->Threshold = PencilSketchParameters.Threshold;
			PassParameters->Sensitivity = PencilSketchParameters.Sensitivity;
			PassParameters->Boldness = PencilSketchParameters.Boldness;

			PassParameters->InColorTexture = GraphBuilder.CreateSRV(InColorTexture);
		}
	);

	return OutTexture;
}

FRDGTextureRef DownsampleFilter(
	FRDGBuilder& GraphBuilder,
	const FNPRPixelArtParametersProxy& PixelArtParameters,
	FRDGTextureRef InColorTexture
)
{
	// TODO: Still perform dithering
	const int32 NumPasses = PixelArtParameters.NumDownsamplePasses;
	if (NumPasses == 0)
	{
		return InColorTexture;
	}

	FIntPoint Extent = InColorTexture->Desc.Extent;

	FIntRect InRect{ FIntPoint::ZeroValue, Extent };
	FIntRect OutRect{ FIntPoint::ZeroValue, Extent / 2 };


	uint32 WriteTextureIndex = 0;
	FRDGTextureRef PingPongTextures[2] = {
		CreateTextureFrom(GraphBuilder, InColorTexture, TEXT("NPRTools.Downsample.Ping")),
		CreateTextureFrom(GraphBuilder, InColorTexture, TEXT("NPRTools.Downsample.Pong"))
	};

	// Downsample
	for (int32 i = 0; i < NumPasses; i++)
	{
		AddNPRPass<FDownsamplePS>(
			GraphBuilder,
			RDG_EVENT_NAME("Downsample"),
			PingPongTextures[WriteTextureIndex],
			[&](auto PassParameters)
			{
				PassParameters->InColorTexture = GraphBuilder.CreateSRV(
					(i == 0) ? InColorTexture : PingPongTextures[1 - WriteTextureIndex]
				);
			},
			TShaderPermutationDomain(),
			OutRect,
			InRect
		);

		if (i < NumPasses - 1)
		{
			InRect = OutRect;
			OutRect.Max /= 2;
		}

		WriteTextureIndex = (WriteTextureIndex + 1) % 2;
	}

	// Run dithering on the downsampled image
	AddNPRPass<FDitherPS>(
		GraphBuilder,
		RDG_EVENT_NAME("Dither"),
		PingPongTextures[WriteTextureIndex],
		[&](auto PassParameters)
		{
			PassParameters->Spread = PixelArtParameters.DitherSpread;
			PassParameters->ColorCount = PixelArtParameters.ColorCount;
			PassParameters->BayerLevel = PixelArtParameters.BayerLevel;

			PassParameters->InColorTexture = GraphBuilder.CreateSRV(PingPongTextures[1 - WriteTextureIndex]);
		},
		TShaderPermutationDomain(),
		OutRect,
		OutRect
	);

	WriteTextureIndex = (WriteTextureIndex + 1) % 2;

	// Upsample back to full res
	for (int32 i = 0; i < NumPasses; i++)
	{
		AddNPRPass<FDownsamplePS>(
			GraphBuilder,
			RDG_EVENT_NAME("Upsample"),
			PingPongTextures[WriteTextureIndex],
			[&](auto PassParameters)
			{
				PassParameters->InColorTexture = GraphBuilder.CreateSRV(PingPongTextures[1 - WriteTextureIndex]);
			},
			TShaderPermutationDomain(),
			InRect, // These are deliberately swapped for upscaling passes
			OutRect	
		);

		OutRect = InRect;
		InRect.Max *= 2;

		WriteTextureIndex = (WriteTextureIndex + 1) % 2;
	}

	return PingPongTextures[1 - WriteTextureIndex];
}

FRDGTextureRef ShockFilter(
	FRDGBuilder& GraphBuilder,
	const FNPRShockFilterParametersProxy& Parameters,
	FRDGTextureRef InColorTexture,
	FRDGTextureRef InTangentFlowMap,
	FRDGTextureRef InLuminanceTexture
)
{
	FRDGTextureRef SignTexture = CreateTextureFrom(GraphBuilder, InColorTexture, TEXT("NPRTools.ShockFilter.Sign"), 1.0f, PF_R32_FLOAT);
	FRDGTextureRef Output = CreateTextureFrom(GraphBuilder, InColorTexture, TEXT("NPRTools.ShockFilter.Output"));

	FRDGTextureRef Luminance = Parameters.bUseIsotropicBlur ? GaussianBlur(GraphBuilder, InLuminanceTexture, Parameters.SigmaI) : InLuminanceTexture;

	AddNPRPass<FFLoGPS>(
		GraphBuilder,
		RDG_EVENT_NAME("FLoG"),
		SignTexture,
		[&](auto PassParameters)
		{
			PassParameters->Sigma = Parameters.SigmaG;

			PassParameters->InTangentFlowMapTexture = GraphBuilder.CreateSRV(InTangentFlowMap);
			PassParameters->InLuminanceTexture = GraphBuilder.CreateSRV(Luminance);
		}
	);

	AddNPRPass<FGradientShockPS>(
		GraphBuilder,
		RDG_EVENT_NAME("GradientShock"),
		Output,
		[&](auto PassParameters)
		{
			PassParameters->Radius = Parameters.Radius;

			PassParameters->InColorTexture = GraphBuilder.CreateSRV(InColorTexture);
			PassParameters->InTangentFlowMapTexture = GraphBuilder.CreateSRV(InTangentFlowMap);
			PassParameters->InLuminanceTexture = GraphBuilder.CreateSRV(Luminance);
			PassParameters->InSignTexture = GraphBuilder.CreateSRV(SignTexture);
		}
	);

	return Output;
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

	FRDGTextureRef PrevTangentFlowMapTexture = nullptr;
	if (bSmoothTangents)
	{
		PrevTangentFlowMapTexture = GraphBuilder.RegisterExternalTexture(History->PreviousTangentFlowMapTexture);
	}

	// Calculate tangent flow map
	FRDGTextureRef TangentFlowMapTexture = CreateTangentFlowMap(
		GraphBuilder,
		NPRParameters,
		bSmoothTangents,
		InColorTexture,
		PrevTangentFlowMapTexture
	);

	FRDGTextureRef YCCTexture = ConvertToYCC(GraphBuilder, InColorTexture);

	FRDGTextureRef BilateralTexture = BilateralFilter(
		GraphBuilder,
		NPRParameters.BilateralFilterParameters,
		YCCTexture,
		TangentFlowMapTexture
	);

	FRDGTextureRef DoGTexture = DifferenceOfGaussians(
		GraphBuilder,
		NPRParameters.DoGParameters,
		BilateralTexture,
		TangentFlowMapTexture
	);

	// Process colour
	FRDGTextureRef ProcessedColorTexture = InColorTexture;
	if (NPRParameters.bCompositeColor)
	{
		switch (NPRParameters.ColorPipeline)
		{
		case ENPRToolsColorPipeline::Quantization:
			ProcessedColorTexture = QuantizeFilter(
				GraphBuilder,
				NPRParameters.QuantizationParameters,
				BilateralTexture
			);
			break;
		case ENPRToolsColorPipeline::Kuwahara:
			ProcessedColorTexture = KuwaharaFilter(
				GraphBuilder,
				NPRParameters.KuwaharaParameters,
				InColorTexture,
				TangentFlowMapTexture
			);
			break;
		case ENPRToolsColorPipeline::OilPaint:
			ProcessedColorTexture = OilPaintFilter(
				GraphBuilder,
				NPRParameters.OilPaintParameters,
				InColorTexture
			);
			break;
		case ENPRToolsColorPipeline::PencilSketch:
			ProcessedColorTexture = PencilSketchFilter(
				GraphBuilder,
				NPRParameters.PencilSketchParameters,
				InColorTexture
			);
			break;
		case ENPRToolsColorPipeline::PixelArt:
			ProcessedColorTexture = DownsampleFilter(
				GraphBuilder,
				NPRParameters.PixelArtParameters,
				InColorTexture
			);
			break;
		case ENPRToolsColorPipeline::ShockFilter:
			ProcessedColorTexture = ShockFilter(
				GraphBuilder,
				NPRParameters.ShockFilterParameters,
				InColorTexture,
				TangentFlowMapTexture,
				YCCTexture
			);
			break;
		default:
			// No color processing will retain original scene colour
			break;
		}
	}

	// Image composition
	if (NPRParameters.bCompositeColor && NPRParameters.bCompositeEdges)
	{
		// Combine edges and quantized color
		AddNPRPass<FCombineEdgesPassPS>(
			GraphBuilder,
			RDG_EVENT_NAME("CombineEdges"),
			OutColorTexture,
			[&](auto PassParameters)
			{
				PassParameters->InColorTexture = GraphBuilder.CreateSRV(ProcessedColorTexture);
				PassParameters->InEdgesTexture = GraphBuilder.CreateSRV(DoGTexture);
			}
		);
	}
	else if (NPRParameters.bCompositeColor)
	{
		AddCopyTexturePass(GraphBuilder, ProcessedColorTexture, OutColorTexture);
	}
	else if (NPRParameters.bCompositeEdges)
	{
		AddCopyTexturePass(GraphBuilder, DoGTexture, OutColorTexture);
	}
	else
	{
		UE_LOG(LogNPRTools, Error, TEXT("Parameter configuration error: Invalid composition mode!"));
	}

	// Save tangent flow map for next frame for temporal smoothing
	// TODO: Not if history exists, but if pointer to history object is valid
	if (bHistoryExists)
	{
		GraphBuilder.QueueTextureExtraction(TangentFlowMapTexture, &History->PreviousTangentFlowMapTexture);
	}

	return true;
}
