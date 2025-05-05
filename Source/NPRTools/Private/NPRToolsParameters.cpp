// Fill out your copyright notice in the Description page of Project Settings.


#include "NPRToolsParameters.h"

UNPRToolsParametersDataAsset::UNPRToolsParametersDataAsset()
{
	bEnable = true;

	NumBilateralFilterPasses = 1;

	TangentSigmaD = 3.0f;
	TangentSigmaR = 0.425f;
	GradientSigmaD = 0.34f;
	GradientSigmaR = 3.3f;

	bSmoothTangents = true;
	SmoothingAmount = 0.9f;

	SigmaE = 1.0f;
	K = 1.6f;
	Tau = 20.0f;

	SigmaM = 3.0f;

	ThresholdingMethod = ENPRToolsDifferenceOfGaussiansThresholdingMethod::HyperbolicTangent;
	Epsilon = 0.1f;
	PhiEdge = 3.4f;

	bEnableQuantization = true;
	NumBins = 16;
	PhiColor = 3.4f;

	bUseKuwahara = false;
	KuwaharaKernelSize = 4;
	KuwaharaHardness = 8.0f;
	KuwaharaSharpness = 8.0f;
	KuwaharaAlpha = 1.0f;
	KuwaharaZeroCrossing = 0.58f;
	KuwaharaZeta = 0.1f;

	bUseOilPaint = false;
	OilPaintBrushDetail = 1.0f;
	OilPaintStrokeBend = -1.0f;
	OilPaintBrushSize = 1.0f;
	bOilPaintEnableReliefLighting = true;
	OilPaintPaintSpecular = 0.75f;

	bCompositeColor = true;
	bCompositeEdges = true;
}
