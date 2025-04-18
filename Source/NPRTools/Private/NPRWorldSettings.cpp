// Fill out your copyright notice in the Description page of Project Settings.


#include "NPRWorldSettings.h"

const UNPRToolsParametersDataAsset* ANPRWorldSettings::GetNPRToolsParameters() const
{
	return NPRParametersAsset.Get();
}
