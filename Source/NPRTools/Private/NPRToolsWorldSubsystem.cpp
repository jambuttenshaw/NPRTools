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
	ANPRWorldSettings* WorldSettings = Cast<ANPRWorldSettings>(GetWorld()->GetWorldSettings());
	if (!WorldSettings)
	{
		// TODO: Might be better to explicitly set an empty proxy?
		return;
	}

	TSharedPtr<FNPRToolsParametersProxy> Proxy = CreateProxyFromAsset(WorldSettings->NPRParametersAsset);

	ENQUEUE_RENDER_COMMAND(CopyNPRToolsParametersProxies)(
		[this, TempProxy = MoveTemp(Proxy)]
		(FRHICommandListImmediate& RHICmdList)
	{
		ParametersProxy = TempProxy;
	});
}


FNPRToolsParametersProxyPtr UNPRToolsWorldSubsystem::CreateProxyFromAsset(const UNPRToolsParametersDataAsset* ParamsAsset) const
{
	FNPRToolsParametersProxyPtr TempProxy;
	if (ParamsAsset)
	{
		TempProxy = MakeShared<FNPRToolsParametersProxy>();

		TempProxy->SigmaD1 = ParamsAsset->SigmaD1;
		TempProxy->SigmaR1 = ParamsAsset->SigmaR1;
		TempProxy->SigmaD2 = ParamsAsset->SigmaD2;
		TempProxy->SigmaR2 = ParamsAsset->SigmaR2;

		TempProxy->SigmaE = ParamsAsset->SigmaE;
		TempProxy->K = ParamsAsset->K;
		TempProxy->Tau = ParamsAsset->Tau;

		TempProxy->SigmaM = ParamsAsset->SigmaM;

		TempProxy->Epsilon = ParamsAsset->Epsilon;
		TempProxy->PhiEdge = ParamsAsset->PhiEdge;

		TempProxy->bEnableQuantization = ParamsAsset->bEnableQuantization;
		TempProxy->NumBins = ParamsAsset->NumBins;
		TempProxy->PhiColor = ParamsAsset->PhiColor;

		TempProxy->bNoEdges = ParamsAsset->bNoEdges;
		TempProxy->bEdgesOnly = ParamsAsset->bEdgesOnly;
	}

	return TempProxy;
}
