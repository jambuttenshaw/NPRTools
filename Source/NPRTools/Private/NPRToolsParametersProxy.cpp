#include "NPRToolsParametersProxy.h"


FNPRToolsParametersProxy::FNPRToolsParametersProxy(const UNPRToolsParametersDataAsset* ParamsAsset)
{
	check(ParamsAsset);

	bEnable = ParamsAsset->bEnable;

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

	bEnableQuantization = ParamsAsset->bEnableQuantization;
	NumBins = ParamsAsset->NumBins;
	PhiColor = ParamsAsset->PhiColor;

	bUseKuwahara = ParamsAsset->bUseKuwahara;
	KuwaharaKernelSize = ParamsAsset->KuwaharaKernelSize;
	KuwaharaHardness = ParamsAsset->KuwaharaHardness;
	KuwaharaSharpness = ParamsAsset->KuwaharaSharpness;
	KuwaharaAlpha = ParamsAsset->KuwaharaAlpha;
	KuwaharaZeroCrossing = ParamsAsset->KuwaharaZeroCrossing;
	KuwaharaZeta = ParamsAsset->KuwaharaZeta;

	bCompositeColor = ParamsAsset->bCompositeColor;
	bCompositeEdges = ParamsAsset->bCompositeEdges;
}

