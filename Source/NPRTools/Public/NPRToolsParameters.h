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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Tangents")
	bool bSmoothTangents;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Tangents", meta = (ClampMin = "0.0", ClampMax = "0.99"))
	float SmoothingAmount;

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


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Oil Paint")
	bool bUseOilPaint;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Oil Paint")
	float OilPaintBrushDetail;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Oil Paint")
	float OilPaintStrokeBend;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Oil Paint")
	float OilPaintBrushSize;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Oil Paint")
	bool bOilPaintEnableReliefLighting;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Oil Paint")
	float OilPaintPaintSpecular;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pencil Sketch")
	bool bUsePencilSketch;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pencil Sketch")
	float PencilSketchThreshold;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pencil Sketch")
	float PencilSketchSensitivity;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pencil Sketch")
	float PencilSketchBoldness;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Composition")
	bool bCompositeColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Composition")
	bool bCompositeEdges;
};
