// Fill out your copyright notice in the Description page of Project Settings.


#include "NPRToolsParameters.h"

UNPRToolsParametersDataAsset::UNPRToolsParametersDataAsset()
{
	bEnable = true;
	bCompositeColor = true;
	bCompositeEdges = true;

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

	ColorPipeline = ENPRToolsColorPipeline::Quantization;

	QuantizationParameters.NumBins = 16;
	QuantizationParameters.PhiColor = 3.4f;

	KuwaharaParameters.KernelSize = 4;
	KuwaharaParameters.Hardness = 8.0f;
	KuwaharaParameters.Sharpness = 8.0f;
	KuwaharaParameters.Alpha = 1.0f;
	KuwaharaParameters.ZeroCrossing = 0.58f;
	KuwaharaParameters.Zeta = 0.1f;

	OilPaintParameters.BrushDetail = 1.0f;
	OilPaintParameters.StrokeBend = -1.0f;
	OilPaintParameters.BrushSize = 1.0f;
	OilPaintParameters.bEnableReliefLighting = true;
	OilPaintParameters.PaintSpecular = 0.75f;

	PencilSketchParameters.Threshold = 0.01f;
	PencilSketchParameters.Sensitivity = 10.0f;
	PencilSketchParameters.Boldness = 0.9f;
}
