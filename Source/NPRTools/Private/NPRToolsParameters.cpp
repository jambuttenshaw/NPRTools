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

	SigmaE = 1.0f;
	K = 1.6f;
	Tau = 20.0f;

	SigmaM = 3.0f;

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

	CompositionMode = ENPRToolsCompositionMode::ColourAndEdges;
}
