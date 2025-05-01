// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameFramework/WorldSettings.h"

#include "NPRToolsParameters.h"

#include "NPRWorldSettings.generated.h"


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
