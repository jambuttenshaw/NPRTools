#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NPRToolsParameters.generated.h"


UENUM(BlueprintType)
enum class ENPRToolsDifferenceOfGaussiansThresholdingMethod: uint8
{
	Binary = 0,
	HyperbolicTangent = 1
};


/**
 *
 */
UCLASS(BlueprintType)
class NPRTOOLS_API UNPRToolsParametersDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UNPRToolsParametersDataAsset();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Global")
	bool bEnable;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Bilateral Filter")
	int32 NumBilateralFilterPasses;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Bilateral Filter")
	float TangentSigmaD;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Bilateral Filter")
	float TangentSigmaR;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Bilateral Filter")
	float GradientSigmaD;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Bilateral Filter")
	float GradientSigmaR;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Difference of Gaussians")
	float SigmaE;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Difference of Gaussians")
	float K;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Difference of Gaussians")
	float Tau;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Flow-Based Difference of Gaussians")
	float SigmaM;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Difference of Gaussians Thresholding")
	ENPRToolsDifferenceOfGaussiansThresholdingMethod ThresholdingMethod;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Difference of Gaussians Thresholding")
	float Epsilon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Difference of Gaussians Thresholding")
	float PhiEdge;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Color Quantization")
	bool bEnableQuantization;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Color Quantization")
	int32 NumBins;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Color Quantization")
	float PhiColor;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kuwahara")
	bool bUseKuwahara;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kuwahara")
	int32 KuwaharaKernelSize;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kuwahara")
	float KuwaharaHardness;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kuwahara")
	float KuwaharaSharpness;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kuwahara")
	float KuwaharaAlpha;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kuwahara")
	float KuwaharaZeroCrossing;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kuwahara")
	float KuwaharaZeta;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Composition")
	bool bCompositeColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Composition")
	bool bCompositeEdges;
};


// Render thread representation of parameters controlling NPR FX
struct FNPRToolsParametersProxy
{
	// Global parameters
	bool bEnable;

	// Bilateral filter parameters
	int32 NumBilateralFilterPasses;
	float SigmaD1;
	float SigmaR1;
	float SigmaD2;
	float SigmaR2;

	// Difference of Gaussian parameters
	float SigmaE;
	float K;
	float Tau;

	// Flow-based DoG parameters
	float SigmaM;

	// Thresholding parameters
	ENPRToolsDifferenceOfGaussiansThresholdingMethod ThresholdingMethod;
	float Epsilon;
	float PhiEdge;

	// Quantization parameters
	bool bEnableQuantization;
	int32 NumBins;
	float PhiColor;

	bool bUseKuwahara;
	int32 KuwaharaKernelSize;
	float KuwaharaHardness;
	float KuwaharaSharpness;
	float KuwaharaAlpha;
	float KuwaharaZeroCrossing;
	float KuwaharaZeta;

	// Composition parameters
	bool bCompositeColor;
	bool bCompositeEdges;
};


using FNPRToolsParametersProxyPtr = TSharedPtr<FNPRToolsParametersProxy, ESPMode::ThreadSafe>;
