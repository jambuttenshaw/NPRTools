#include "NPRToolsWorldSubsystem.h"

#include "SceneViewExtension.h"
#include "NPRToolsViewExtension.h"
#include "NPRWorldSettings.h"


void UNPRToolsWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	// View extension lifetime is equal to the UWorld lifetime
	ViewExtension = FSceneViewExtensions::NewExtension<FNPRToolsViewExtension>(this);
}

void UNPRToolsWorldSubsystem::Deinitialize()
{
	ViewExtension->Invalidate();
}


void UNPRToolsWorldSubsystem::TransferState()
{
	// Get params asset from world settings
	const UNPRToolsParametersDataAsset* ParamsAsset = nullptr;

	if (ParamsAssetOverride)
	{
		ParamsAsset = ParamsAssetOverride;
	}
	else if (AWorldSettings* WorldSettings = GetWorld()->GetWorldSettings())
	{
		if (INPRWorldSettingsInterface* NPRSettingsInterface = Cast<INPRWorldSettingsInterface>(WorldSettings))
		{
			ParamsAsset = NPRSettingsInterface->GetNPRToolsParameters();
		}
	}

	TSharedPtr<FNPRToolsParametersProxy> Proxy = CreateProxyFromAsset(ParamsAsset);

	ENQUEUE_RENDER_COMMAND(CopyNPRToolsParametersProxies)(
		[this, TempProxy = MoveTemp(Proxy)]
		(FRHICommandListImmediate& RHICmdList)
	{
		ParametersProxy = TempProxy;
	});
}

void UNPRToolsWorldSubsystem::OverrideNPRParametersAsset(UNPRToolsParametersDataAsset* ParamsAsset)
{
	ParamsAssetOverride = ParamsAsset;
}


FNPRToolsParametersProxyPtr UNPRToolsWorldSubsystem::CreateProxyFromAsset(const UNPRToolsParametersDataAsset* ParamsAsset) const
{
	FNPRToolsParametersProxyPtr TempProxy;
	if (ParamsAsset)
	{
		TempProxy = MakeShared<FNPRToolsParametersProxy>();

		TempProxy->bEnable = ParamsAsset->bEnable;

		TempProxy->NumBilateralFilterPasses = ParamsAsset->NumBilateralFilterPasses;
		TempProxy->SigmaD1 = ParamsAsset->TangentSigmaD;
		TempProxy->SigmaR1 = ParamsAsset->TangentSigmaR;
		TempProxy->SigmaD2 = ParamsAsset->GradientSigmaD;
		TempProxy->SigmaR2 = ParamsAsset->GradientSigmaR;

		TempProxy->SigmaE = ParamsAsset->SigmaE;
		TempProxy->K = ParamsAsset->K;
		TempProxy->Tau = ParamsAsset->Tau;

		TempProxy->SigmaM = ParamsAsset->SigmaM;

		TempProxy->ThresholdingMethod = ParamsAsset->ThresholdingMethod;
		TempProxy->Epsilon = ParamsAsset->Epsilon;
		TempProxy->PhiEdge = ParamsAsset->PhiEdge;

		TempProxy->bEnableQuantization = ParamsAsset->bEnableQuantization;
		TempProxy->NumBins = ParamsAsset->NumBins;
		TempProxy->PhiColor = ParamsAsset->PhiColor;

		TempProxy->bUseKuwahara = ParamsAsset->bUseKuwahara;
		TempProxy->KuwaharaKernelSize = ParamsAsset->KuwaharaKernelSize;
		TempProxy->KuwaharaHardness = ParamsAsset->KuwaharaHardness;
		TempProxy->KuwaharaSharpness = ParamsAsset->KuwaharaSharpness;
		TempProxy->KuwaharaAlpha = ParamsAsset->KuwaharaAlpha;
		TempProxy->KuwaharaZeroCrossing = ParamsAsset->KuwaharaZeroCrossing;
		TempProxy->KuwaharaZeta = ParamsAsset->KuwaharaZeta;

		TempProxy->bCompositeColor = ParamsAsset->bCompositeColor;
		TempProxy->bCompositeEdges = ParamsAsset->bCompositeEdges;
	}

	return TempProxy;
}
