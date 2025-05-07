#pragma once

#include "CoreMinimal.h"
#include "NPRToolsParameters.h"

#include "CompositingElements/CompositingElementPasses.h"
#include "NPRCompositingElementTransforms.generated.h"


UCLASS(BlueprintType, Blueprintable)
class NPRTOOLS_API UCompositingNPRPass : public UCompositingElementTransform
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Compositing Pass", meta = (DisplayAfter = "PassName", EditCondition = "bEnabled"))
	UNPRToolsParametersDataAsset* NPRParameters;

public:
	virtual UTexture* ApplyTransform_Implementation(UTexture* Input, UComposurePostProcessingPassProxy* PostProcessProxy, ACameraActor* TargetCamera) override;

private:

	void ApplyTransform_RenderThread(FRHICommandListImmediate& RHICmdList, FTextureResource* InputResource, FTextureResource* RenderTargetResource) const;

private:
	TSharedPtr<struct FNPRToolsParametersProxy, ESPMode::ThreadSafe> ParametersProxy;

};
