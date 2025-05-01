#pragma once

#include "CoreMinimal.h"

#include "CompositingElements/CompositingElementPasses.h"
#include "NPRCompositingElementTransforms.generated.h"


UCLASS(BlueprintType, Blueprintable)
class NPRTOOLS_API UCompositingNPRPass : public UCompositingElementTransform
{
	GENERATED_BODY()

public:

public:
	virtual UTexture* ApplyTransform_Implementation(UTexture* Input, UComposurePostProcessingPassProxy* PostProcessProxy, ACameraActor* TargetCamera) override;

};
