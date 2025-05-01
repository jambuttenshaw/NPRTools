#pragma once


// History on the previous NPR pipeline invocation to allow temporal smoothing
// If multiple pipeline invocations are performed in a frame, 
// a separate history should be maintained for each temporally-coherent invocation
struct FNPRToolsHistory
{
	TRefCountPtr<IPooledRenderTarget> PreviousTangentFlowMapTexture;


	bool IsValid()
	{
		bool bValid = true;
		bValid &= PreviousTangentFlowMapTexture.IsValid();
		return bValid;
	}
};
