// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"

#include "NPRToolsParameters.h"

#include "NPRWorldSettings.generated.h"

/**
 * 
 */
UCLASS()
class NPRTOOLS_API ANPRWorldSettings : public AWorldSettings
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NPR Tools")
	TObjectPtr<UNPRToolsParametersDataAsset> NPRParametersAsset;
};
