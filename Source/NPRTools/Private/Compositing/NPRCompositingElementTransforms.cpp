#include "Compositing/NPRCompositingElementTransforms.h"

#include "NPRToolsWorldSubsystem.h"
#include "NPRToolsParametersProxy.h"

#include "Pipelines/NPRPipeline.h"

#include "RenderGraphBuilder.h"
#include "Rendering/Texture2DResource.h"


UTexture* UCompositingNPRPass::ApplyTransform_Implementation(UTexture* Input, UComposurePostProcessingPassProxy* PostProcessProxy, ACameraActor* TargetCamera)
{
	UTexture* Result = Input;

	if (NPRParameters)
	{
		// Create parameters proxy
		FNPRToolsParametersProxyPtr Proxy = MakeShared<FNPRToolsParametersProxy>(NPRParameters);

		// Validate input colour texture
		check(Input);
		check(Input->GetResource());

		FIntPoint Dims;
		Dims.X = Input->GetResource()->GetSizeX();
		Dims.Y = Input->GetResource()->GetSizeY();

		EPixelFormat Format = PF_Unknown;
		if (UTextureRenderTarget2D* InputRT = Cast<UTextureRenderTarget2D>(Input))
		{
			Format = InputRT->GetFormat();
		}
		else if (UTexture2D* InputTexture2D = Cast<UTexture2D>(Input))
		{
			Format = InputTexture2D->GetPixelFormat();
		}
		else
		{
			check(false);
		}

		// Get a render target to output to
		UTextureRenderTarget2D* RenderTarget = RequestRenderTarget(Dims, Format);
		check(RenderTarget);
		check(RenderTarget->GetResource());

		ENQUEUE_RENDER_COMMAND(ApplyNPRTransform)(
				[this, TempProxy = MoveTemp(Proxy), InputResource = Input->GetResource(), OutputResource = RenderTarget->GetResource()]
				(FRHICommandListImmediate& RHICmdList)
			{
				this->ParametersProxy = TempProxy;
				this->ApplyTransform_RenderThread(RHICmdList, InputResource, OutputResource);
			}
		);

		Result = RenderTarget;
	}

	return Result;
}


void UCompositingNPRPass::ApplyTransform_RenderThread(FRHICommandListImmediate& RHICmdList, FTextureResource* InputResource, FTextureResource* RenderTargetResource) const
{
	check(IsInRenderingThread());
	check(ParametersProxy);

	FRDGBuilder GraphBuilder(RHICmdList);

	TRefCountPtr<IPooledRenderTarget> InputRT = CreateRenderTarget(InputResource->GetTextureRHI(), TEXT("CompositingNPRPass.Input"));
	TRefCountPtr<IPooledRenderTarget> OutputRT = CreateRenderTarget(RenderTargetResource->GetTextureRHI(), TEXT("CompositingNPRPass.Output"));

	// Set up RDG resources
	FRDGTextureRef InColorTexture = GraphBuilder.RegisterExternalTexture(InputRT);
	FRDGTextureRef OutColorTexture = GraphBuilder.RegisterExternalTexture(InputRT);

	// Execute pipeline
	NPRTools::ExecuteNPRPipeline(
		GraphBuilder,
		*ParametersProxy,
		InColorTexture,
		OutColorTexture,
		nullptr
	);

	GraphBuilder.Execute();
}
