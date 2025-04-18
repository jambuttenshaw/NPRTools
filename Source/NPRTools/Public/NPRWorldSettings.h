// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"

#include "NPRToolsParameters.h"
#include "NPRWorldSettingsInterface.h"

#include "NPRWorldSettings.generated.h"

/**
 * 
 */
UCLASS()
class NPRTOOLS_API ANPRWorldSettings : public AWorldSettings, public INPRWorldSettingsInterface
{
	GENERATED_BODY()
public:
	virtual const UNPRToolsParametersDataAsset* GetNPRToolsParameters() const override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NPR Tools")
	TObjectPtr<UNPRToolsParametersDataAsset> NPRParametersAsset;

};
