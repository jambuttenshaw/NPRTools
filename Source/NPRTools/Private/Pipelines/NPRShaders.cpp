#include "NPRShaders.h"


IMPLEMENT_GLOBAL_SHADER(FSobelPassPS, "/NPRTools/Sobel.usf", "SobelPS", SF_Pixel);

IMPLEMENT_GLOBAL_SHADER(FBlurEigenVerticalPassPS, "/NPRTools/EigenBlur.usf", "BlurEigenVerticalPS", SF_Pixel);

IMPLEMENT_GLOBAL_SHADER(FBlurEigenHorizontalPassPS, "/NPRTools/EigenBlur.usf", "BlurEigenHorizontalPS", SF_Pixel);

IMPLEMENT_GLOBAL_SHADER(FConvertYCCPassPS, "/NPRTools/ConvertYCC.usf", "ConvertYCbCrPS", SF_Pixel);

IMPLEMENT_GLOBAL_SHADER(FBilateralPassPS, "/NPRTools/Bilateral.usf", "BilateralPS", SF_Pixel);

IMPLEMENT_GLOBAL_SHADER(FDoGGradientPassPS, "/NPRTools/DoGGradient.usf", "DoGGradientPS", SF_Pixel);

IMPLEMENT_GLOBAL_SHADER(FDoGFlowPassPS, "/NPRTools/DoGFlow.usf", "DoGFlowPS", SF_Pixel);

IMPLEMENT_GLOBAL_SHADER(FQuantizePassPS, "/NPRTools/Quantize.usf", "QuantizePS", SF_Pixel);

IMPLEMENT_GLOBAL_SHADER(FKuwaharaPassPS, "/NPRTools/Kuwahara.usf", "KuwaharaPS", SF_Pixel);

IMPLEMENT_GLOBAL_SHADER(FCombineEdgesPassPS, "/NPRTools/CombineEdges.usf", "CombineEdgesPS", SF_Pixel);

IMPLEMENT_GLOBAL_SHADER(FOilPaintStrokesPS, "/NPRTools/OilPaint.usf", "OilPaintStrokesPS", SF_Pixel);

IMPLEMENT_GLOBAL_SHADER(FOilPaintReliefLightingPS, "/NPRTools/OilPaint.usf", "OilPaintReliefLightingPS", SF_Pixel);

IMPLEMENT_GLOBAL_SHADER(FPencilSketchPS, "/NPRTools/PencilSketch.usf", "PencilSketchPS", SF_Pixel);

IMPLEMENT_GLOBAL_SHADER(FDownsamplePS, "/NPRTools/Downsample.usf", "DownsamplePS", SF_Pixel);
