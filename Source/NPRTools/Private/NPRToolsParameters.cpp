// Fill out your copyright notice in the Description page of Project Settings.


#include "NPRToolsParameters.h"

UNPRToolsParametersDataAsset::UNPRToolsParametersDataAsset()
{
	SigmaD1 = 3.0f;
	SigmaR1 = 0.425f;
	SigmaD2 = 0.34f;
	SigmaR2 = 3.3f;

	SigmaE = 1.0f;
	K = 1.6f;
	Tau = 20.0f;

	SigmaM = 3.0f;

	Epsilon = 0.1f;
	PhiEdge = 3.4f;

	bEnableQuantization = true;
	NumBins = 16;
	PhiColor = 3.4f;

	bNoEdges = false;
	bEdgesOnly = false;
}
