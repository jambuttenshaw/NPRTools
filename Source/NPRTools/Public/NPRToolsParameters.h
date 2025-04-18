#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NPRToolsParameters.generated.h"


/* Some workable defaults:
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

	NumBins = 16;
	PhiColor = 3.4f;
	 */


UENUM(BlueprintType)
enum class ENPRToolsCompositionMode : uint8
{
	ColourOnly = 0,
	EdgesOnly = 1,
	ColourAndEdges = 2
};


/**
 *
 */
UCLASS()
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
	float Epsilon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Difference of Gaussians Thresholding")
	float PhiEdge;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Color Quantization")
	bool bEnableQuantization;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Color Quantization")
	int32 NumBins;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Color Quantization")
	float PhiColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Color Quantization")
	bool bUseKuwahara;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Color Quantization")
	bool bAnisotropicKuwahara;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Color Quantization")
	float KuwaharaRadius;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Color Quantization")
	float KuwaharaTuning;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Composition")
	ENPRToolsCompositionMode CompositionMode;
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
	float Epsilon;
	float PhiEdge;

	// Quantization parameters
	bool bEnableQuantization;
	int32 NumBins;
	float PhiColor;

	bool bUseKuwahara;
	bool bAnisotropicKuwahara;
	float KuwaharaRadius;
	float KuwaharaTuning;

	// Composition parameters
	ENPRToolsCompositionMode CompositionMode;
};


using FNPRToolsParametersProxyPtr = TSharedPtr<FNPRToolsParametersProxy, ESPMode::ThreadSafe>;
