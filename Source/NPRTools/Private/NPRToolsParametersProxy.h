#pragma once

#include "NPRToolsParameters.h"


struct FNPRBilateralFilterParametersProxy
{
	int32 NumPasses;
	float SigmaD1;
	float SigmaR1;
	float SigmaD2;
	float SigmaR2;
};

struct FNPRDifferenceOfGaussiansParametersProxy
{
	float SigmaE;
	float K;
	float Tau;
	float SigmaM;
	ENPRToolsDifferenceOfGaussiansThresholdingMethod ThresholdingMethod;
	float Epsilon;
	float PhiEdge;
};

struct FNPRQuantizationParametersProxy
{
	int32 NumBins;
	float PhiColor;
};

struct FNPRKuwaharaParametersProxy
{
	int32 KernelSize;
	float Hardness;
	float Sharpness;
	float Alpha;
	float ZeroCrossing;
	float Zeta;
};

struct FNPROilPaintParametersProxy
{
	float BrushDetail;
	float StrokeBend;
	float BrushSize;
	bool bEnableReliefLighting;
	float PaintSpecular;
};

struct FNPRPencilSketchParametersProxy
{
	float Threshold;
	float Sensitivity;
	float Boldness;
};

// Render thread representation of parameters controlling NPR FX
struct FNPRToolsParametersProxy
{
	// Global parameters
	bool bEnable;
	bool bCompositeColor;
	bool bCompositeEdges;

	FNPRBilateralFilterParametersProxy BilateralFilterParameters;

	// Tangent parameters
	bool bSmoothTangents;
	float SmoothingAmount;

	FNPRDifferenceOfGaussiansParametersProxy DoGParameters;

	ENPRToolsColorPipeline ColorPipeline;

	// Only one pipeline is in effect at a time
	union
	{
		FNPRQuantizationParametersProxy QuantizationParameters;
		FNPRKuwaharaParametersProxy KuwaharaParameters;
		FNPROilPaintParametersProxy OilPaintParameters;
		FNPRPencilSketchParametersProxy PencilSketchParameters;
	};


	explicit FNPRToolsParametersProxy(const UNPRToolsParametersDataAsset* ParamsAsset);
};
