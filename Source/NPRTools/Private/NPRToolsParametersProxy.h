#pragma once

#include "NPRToolsParameters.h"


// Render thread representation of parameters controlling NPR FX
struct FNPRToolsParametersProxy
{
	// Global parameters
	bool bEnable;

	// Bilateral filter parameters
	int32 NumBilateralFilterPasses;
	float SigmaD1;
	float SigmaR1;
	float SigmaD2;
	float SigmaR2;

	// Tangent parameters
	bool bSmoothTangents;
	float SmoothingAmount;

	// Difference of Gaussian parameters
	float SigmaE;
	float K;
	float Tau;

	// Flow-based DoG parameters
	float SigmaM;

	// Thresholding parameters
	ENPRToolsDifferenceOfGaussiansThresholdingMethod ThresholdingMethod;
	float Epsilon;
	float PhiEdge;

	// Quantization parameters
	bool bEnableQuantization;
	int32 NumBins;
	float PhiColor;

	bool bUseKuwahara;
	int32 KuwaharaKernelSize;
	float KuwaharaHardness;
	float KuwaharaSharpness;
	float KuwaharaAlpha;
	float KuwaharaZeroCrossing;
	float KuwaharaZeta;

	// Composition parameters
	bool bCompositeColor;
	bool bCompositeEdges;

	explicit FNPRToolsParametersProxy(const UNPRToolsParametersDataAsset* ParamsAsset);
};
