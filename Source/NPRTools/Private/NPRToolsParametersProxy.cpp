#include "NPRToolsParametersProxy.h"


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

	NumBilateralFilterPasses = ParamsAsset->NumBilateralFilterPasses;
	SigmaD1 = ParamsAsset->TangentSigmaD;
	SigmaR1 = ParamsAsset->TangentSigmaR;
	SigmaD2 = ParamsAsset->GradientSigmaD;
	SigmaR2 = ParamsAsset->GradientSigmaR;

	bSmoothTangents = ParamsAsset->bSmoothTangents;
	SmoothingAmount = ParamsAsset->SmoothingAmount;

	SigmaE = ParamsAsset->SigmaE;
	K = ParamsAsset->K;
	Tau = ParamsAsset->Tau;

	SigmaM = ParamsAsset->SigmaM;

	ThresholdingMethod = ParamsAsset->ThresholdingMethod;
	Epsilon = ParamsAsset->Epsilon;
	PhiEdge = ParamsAsset->PhiEdge;

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
	default:
		break;
	}
}
