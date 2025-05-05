#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NPRToolsParameters.generated.h"


UENUM(BlueprintType)
enum class ENPRToolsColorPipeline : uint8
{
	None = 0,
	Quantization,
	Kuwahara,
	OilPaint,
	PencilSketch
};

UENUM(BlueprintType)
enum class ENPRToolsDifferenceOfGaussiansThresholdingMethod : uint8
{
	Binary = 0,
	HyperbolicTangent = 1
};


USTRUCT(BlueprintType)
struct FNPRQuantizationParameters
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 NumBins;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float PhiColor;
};

USTRUCT(BlueprintType)
struct FNPRKuwaharaParameters
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 KernelSize;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Hardness;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Sharpness;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Alpha;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ZeroCrossing;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Zeta;
};

USTRUCT(BlueprintType)
struct FNPROilPaintParameters
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float BrushDetail;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float StrokeBend;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float BrushSize;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bEnableReliefLighting;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float PaintSpecular;
};

USTRUCT(BlueprintType)
struct FNPRPencilSketchParameters
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Threshold;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Sensitivity;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Boldness;
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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Global")
	bool bCompositeColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Global")
	bool bCompositeEdges;


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


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Color")
	ENPRToolsColorPipeline ColorPipeline;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Quantization",
		meta = (ShowOnlyInnerProperties, EditCondition = "bCompositeColor&&ColorPipeline==ENPRToolsColorPipeline::Quantization"))
	FNPRQuantizationParameters QuantizationParameters;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kuwahara",
		meta = (ShowOnlyInnerProperties, EditCondition = "bCompositeColor&&ColorPipeline==ENPRToolsColorPipeline::Kuwahara"))
	FNPRKuwaharaParameters KuwaharaParameters;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Oil Paint", 
		meta=(ShowOnlyInnerProperties, EditCondition="bCompositeColor&&ColorPipeline==ENPRToolsColorPipeline::OilPaint"))
	FNPROilPaintParameters OilPaintParameters;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PencilSketch",
		meta = (ShowOnlyInnerProperties, EditCondition = "bCompositeColor&&ColorPipeline==ENPRToolsColorPipeline::PencilSketch"))
	FNPRPencilSketchParameters PencilSketchParameters;
};
