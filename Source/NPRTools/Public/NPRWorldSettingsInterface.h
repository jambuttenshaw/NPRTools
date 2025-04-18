// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NPRWorldSettingsInterface.generated.h"

class UNPRToolsParametersDataAsset;


UINTERFACE()
class UNPRWorldSettingsInterface : public UInterface
{
	GENERATED_BODY()
};

class INPRWorldSettingsInterface
{
	GENERATED_BODY()

public:
	virtual const UNPRToolsParametersDataAsset* GetNPRToolsParameters() const { return nullptr; }
	
};
