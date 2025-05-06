#pragma once

#include "ScreenPass.h"


class FSobelPassPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FSobelPassPS);
	SHADER_USE_PARAMETER_STRUCT(FSobelPassPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, OutViewPort)
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, InViewPort)
		SHADER_PARAMETER_SAMPLER(SamplerState, sampler0)

		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, SceneColorTexture)

		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()
};


class FBlurEigenVerticalPassPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FBlurEigenVerticalPassPS);
	SHADER_USE_PARAMETER_STRUCT(FBlurEigenVerticalPassPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, OutViewPort)
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, InViewPort)
		SHADER_PARAMETER_SAMPLER(SamplerState, sampler0)

		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, InTexture)

		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()
};


class FBlurEigenHorizontalPassPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FBlurEigenHorizontalPassPS);
	SHADER_USE_PARAMETER_STRUCT(FBlurEigenHorizontalPassPS, FGlobalShader);

	class FSmoothTangents : SHADER_PERMUTATION_BOOL("SMOOTH_TANGENTS");
	using FPermutationDomain = TShaderPermutationDomain<FSmoothTangents>;

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, OutViewPort)
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, InViewPort)
		SHADER_PARAMETER_SAMPLER(SamplerState, sampler0)

		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, InTexture)

		SHADER_PARAMETER(float, SmoothingAmount)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, InPrevTangentFlowMap)

		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()
};


class FConvertYCCPassPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FConvertYCCPassPS);
	SHADER_USE_PARAMETER_STRUCT(FConvertYCCPassPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, OutViewPort)
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, InViewPort)
		SHADER_PARAMETER_SAMPLER(SamplerState, sampler0)

		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, SceneColorTexture)

		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()
};


class FBilateralPassPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FBilateralPassPS);
	SHADER_USE_PARAMETER_STRUCT(FBilateralPassPS, FGlobalShader);

	class FBilateralDirectionTangent : SHADER_PERMUTATION_BOOL("BILATERAL_DIRECTION_TANGENT");
	using FPermutationDomain = TShaderPermutationDomain<FBilateralDirectionTangent>;

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, OutViewPort)
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, InViewPort)
		SHADER_PARAMETER_SAMPLER(SamplerState, sampler0)

		SHADER_PARAMETER(float, SigmaD)
		SHADER_PARAMETER(float, SigmaR)

		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, InSceneColorYCCTexture)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, InTangentFlowMapTexture)

		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()
};


class FDoGGradientPassPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FDoGGradientPassPS);
	SHADER_USE_PARAMETER_STRUCT(FDoGGradientPassPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, OutViewPort)
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, InViewPort)
		SHADER_PARAMETER_SAMPLER(SamplerState, sampler0)

		SHADER_PARAMETER(float, SigmaE)
		SHADER_PARAMETER(float, SigmaP)
		SHADER_PARAMETER(float, Tau)

		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, InBilateralTexture)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, InTangentFlowMapTexture)

		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()
};


class FDoGFlowPassPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FDoGFlowPassPS);
	SHADER_USE_PARAMETER_STRUCT(FDoGFlowPassPS, FGlobalShader);

	class FThresholdingMethod : SHADER_PERMUTATION_INT("THRESHOLDING_METHOD", 2);
	using FPermutationDomain = TShaderPermutationDomain<FThresholdingMethod>;

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, OutViewPort)
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, InViewPort)
		SHADER_PARAMETER_SAMPLER(SamplerState, sampler0)

		SHADER_PARAMETER(float, SigmaM)
		SHADER_PARAMETER(float, Epsilon)
		SHADER_PARAMETER(float, Phi)

		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, InDoGGradientTexture)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, InTangentFlowMapTexture)

		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()
};


class FQuantizePassPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FQuantizePassPS);
	SHADER_USE_PARAMETER_STRUCT(FQuantizePassPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, OutViewPort)
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, InViewPort)
		SHADER_PARAMETER_SAMPLER(SamplerState, sampler0)

		SHADER_PARAMETER(int, NumBins)
		SHADER_PARAMETER(float, Phi)

		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, InBilateralTexture)

		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()
};


class FKuwaharaPassPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FKuwaharaPassPS);
	SHADER_USE_PARAMETER_STRUCT(FKuwaharaPassPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, OutViewPort)
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, InViewPort)
		SHADER_PARAMETER_SAMPLER(SamplerState, sampler0)

		SHADER_PARAMETER(int32, KernelSize)
		SHADER_PARAMETER(float, Hardness)
		SHADER_PARAMETER(float, Sharpness)
		SHADER_PARAMETER(float, Alpha)
		SHADER_PARAMETER(float, ZeroCrossing)
		SHADER_PARAMETER(float, Zeta)

		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, SceneColorTexture)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, TangentFlowMapTexture)

		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()
};


class FCombineEdgesPassPS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FCombineEdgesPassPS);
	SHADER_USE_PARAMETER_STRUCT(FCombineEdgesPassPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, OutViewPort)
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, InViewPort)
		SHADER_PARAMETER_SAMPLER(SamplerState, sampler0)

		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, InColorTexture)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, InEdgesTexture)

		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()
};


class FOilPaintStrokesPS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FOilPaintStrokesPS);
	SHADER_USE_PARAMETER_STRUCT(FOilPaintStrokesPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, OutViewPort)
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, InViewPort)
		SHADER_PARAMETER_SAMPLER(SamplerState, sampler0)

		SHADER_PARAMETER(float, BrushDetail)
		SHADER_PARAMETER(float, StrokeBend)
		SHADER_PARAMETER(float, BrushSize)

		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, InColorTexture)

		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()
};


class FOilPaintReliefLightingPS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FOilPaintReliefLightingPS);
	SHADER_USE_PARAMETER_STRUCT(FOilPaintReliefLightingPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, OutViewPort)
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, InViewPort)
		SHADER_PARAMETER_SAMPLER(SamplerState, sampler0)

		SHADER_PARAMETER(float, PaintSpec)

		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, InColorTexture)

		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()
};


class FPencilSketchPS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FPencilSketchPS);
	SHADER_USE_PARAMETER_STRUCT(FPencilSketchPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, OutViewPort)
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, InViewPort)
		SHADER_PARAMETER_SAMPLER(SamplerState, sampler0)

		SHADER_PARAMETER(float, Threshold)
		SHADER_PARAMETER(float, Sensitivity)
		SHADER_PARAMETER(float, Boldness)

		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, InColorTexture)

		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()
};


class FDownsamplePS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FDownsamplePS);
	SHADER_USE_PARAMETER_STRUCT(FDownsamplePS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, OutViewPort)
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, InViewPort)
		SHADER_PARAMETER_SAMPLER(SamplerState, sampler0)

		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, InColorTexture)

		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()
};
