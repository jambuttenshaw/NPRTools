#include "NPRToolsWorldSubsystem.h"

#include "SceneViewExtension.h"
#include "NPRToolsViewExtension.h"
#include "NPRWorldSettings.h"

#include "NPRToolsParametersProxy.h"


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

	FNPRToolsParametersProxyPtr Proxy = MakeShared<FNPRToolsParametersProxy>(ParamsAsset);

	ENQUEUE_RENDER_COMMAND(CopyNPRToolsParametersProxies)(
		[this, TempProxy = MoveTemp(Proxy)]
		(FRHICommandListImmediate&)
	{
		ParametersProxy = TempProxy;
	});
}

void UNPRToolsWorldSubsystem::OverrideNPRParametersAsset(UNPRToolsParametersDataAsset* ParamsAsset)
{
	ParamsAssetOverride = ParamsAsset;
}
