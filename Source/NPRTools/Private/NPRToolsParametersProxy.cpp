#include "NPRToolsParametersProxy.h"


FNPRToolsParametersProxy::FNPRToolsParametersProxy()
{
	FMemory::Memset(this, 0, sizeof(this));
	bEnable = false;
}


FNPRToolsParametersProxy::FNPRToolsParametersProxy(const UNPRToolsParametersDataAsset* ParamsAsset)
{
	// If an invalid params asset is passed in, the proxy will be constructed from the default parameters
	if(!ParamsAsset)
	{
		ParamsAsset = GetDefault<UNPRToolsParametersDataAsset>();
	}

	bEnable = ParamsAsset->bEnable;
	bCompositeColor = ParamsAsset->bCompositeColor;
	bCompositeEdges = ParamsAsset->bCompositeEdges;

	BilateralFilterParameters.NumPasses = ParamsAsset->BilateralFilterParameters.NumPasses;
	BilateralFilterParameters.SigmaD1 = ParamsAsset->BilateralFilterParameters.TangentSigmaD;
	BilateralFilterParameters.SigmaR1 = ParamsAsset->BilateralFilterParameters.TangentSigmaR;
	BilateralFilterParameters.SigmaD2 = ParamsAsset->BilateralFilterParameters.GradientSigmaD;
	BilateralFilterParameters.SigmaR2 = ParamsAsset->BilateralFilterParameters.GradientSigmaR;

	bSmoothTangents = ParamsAsset->bSmoothTangents;
	SmoothingAmount = ParamsAsset->SmoothingAmount;

	DoGParameters.SigmaE = ParamsAsset->DoGParameters.SigmaE;
	DoGParameters.K = ParamsAsset->DoGParameters.K;
	DoGParameters.Tau = ParamsAsset->DoGParameters.Tau;
	DoGParameters.SigmaM = ParamsAsset->DoGParameters.SigmaM;
	DoGParameters.ThresholdingMethod = ParamsAsset->DoGParameters.ThresholdingMethod;
	DoGParameters.Epsilon = ParamsAsset->DoGParameters.Epsilon;
	DoGParameters.PhiEdge = ParamsAsset->DoGParameters.PhiEdge;

	ColorPipeline = ParamsAsset->ColorPipeline;

	switch (ColorPipeline)
	{
	case ENPRToolsColorPipeline::Quantization:
		QuantizationParameters.NumBins = ParamsAsset->QuantizationParameters.NumBins;
		QuantizationParameters.PhiColor = ParamsAsset->QuantizationParameters.PhiColor;
		break;
	case ENPRToolsColorPipeline::Kuwahara:
		KuwaharaParameters.KernelSize = ParamsAsset->KuwaharaParameters.KernelSize;
		KuwaharaParameters.Hardness = ParamsAsset->KuwaharaParameters.Hardness;
		KuwaharaParameters.Sharpness = ParamsAsset->KuwaharaParameters.Sharpness;
		KuwaharaParameters.Alpha = ParamsAsset->KuwaharaParameters.Alpha;
		KuwaharaParameters.ZeroCrossing = ParamsAsset->KuwaharaParameters.ZeroCrossing;
		KuwaharaParameters.Zeta = ParamsAsset->KuwaharaParameters.Zeta;
		break;
	case ENPRToolsColorPipeline::OilPaint:
		OilPaintParameters.BrushDetail = ParamsAsset->OilPaintParameters.BrushDetail;
		OilPaintParameters.StrokeBend = ParamsAsset->OilPaintParameters.StrokeBend;
		OilPaintParameters.BrushSize = ParamsAsset->OilPaintParameters.BrushSize;
		OilPaintParameters.bEnableReliefLighting = ParamsAsset->OilPaintParameters.bEnableReliefLighting;
		OilPaintParameters.PaintSpecular = ParamsAsset->OilPaintParameters.PaintSpecular;
		break;
	case ENPRToolsColorPipeline::PencilSketch:
		PencilSketchParameters.Threshold = ParamsAsset->PencilSketchParameters.Threshold;
		PencilSketchParameters.Sensitivity = ParamsAsset->PencilSketchParameters.Sensitivity;
		PencilSketchParameters.Boldness = ParamsAsset->PencilSketchParameters.Boldness;
		break;
	case ENPRToolsColorPipeline::PixelArt:
		PixelArtParameters.NumDownsamplePasses = ParamsAsset->PixelArtParameters.NumDownsamplePasses;
		PixelArtParameters.DitherSpread = ParamsAsset->PixelArtParameters.DitherSpread;
		PixelArtParameters.ColorCount = ParamsAsset->PixelArtParameters.ColorCount;
		PixelArtParameters.BayerLevel = ParamsAsset->PixelArtParameters.BayerLevel;
		break;
	default:
		break;
	}
}
