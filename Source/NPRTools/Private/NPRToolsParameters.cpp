// Fill out your copyright notice in the Description page of Project Settings.


#include "NPRToolsParameters.h"

UNPRToolsParametersDataAsset::UNPRToolsParametersDataAsset()
{
	bEnable = true;
	bCompositeColor = true;
	bCompositeEdges = true;

	BilateralFilterParameters.NumPasses = 1;
	BilateralFilterParameters.TangentSigmaD = 3.0f;
	BilateralFilterParameters.TangentSigmaR = 0.425f;
	BilateralFilterParameters.GradientSigmaD = 0.34f;
	BilateralFilterParameters.GradientSigmaR = 3.3f;

	bSmoothTangents = true;
	SmoothingAmount = 0.9f;

	DoGParameters.SigmaE = 1.0f;
	DoGParameters.K = 1.6f;
	DoGParameters.Tau = 20.0f;

	DoGParameters.SigmaM = 3.0f;

	DoGParameters.ThresholdingMethod = ENPRToolsDifferenceOfGaussiansThresholdingMethod::HyperbolicTangent;
	DoGParameters.Epsilon = 0.1f;
	DoGParameters.PhiEdge = 3.4f;

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

	DownsampleParameters.NumPasses = 1;
}
