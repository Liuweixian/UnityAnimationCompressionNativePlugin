#include "IUnityAnimationCompression.h"
void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityIssueAnimationCompressionFunc(UnityAnimationCompressionFuncs* callbacks)
{
    callbacks->OnCurveCompress = OnCurveCustomCompress;
    callbacks->OnCurveLoad = OnCurveCustomLoad;
    callbacks->OnCurveUnload = OnCurveCustomUnload;
    callbacks->OnCurveSample = OnCurveCustomSample;
}

int OnCurveCustomCompress(AnimationCurveForPlugin *curve, int curveCount, void *output)
{
    return 10086;
}


void* OnCurveCustomLoad(void* blob, size_t blobSize)
{
    return nullptr;
}

void OnCurveCustomUnload(void* userdata)
{
    
}

void OnCurveCustomSample(void* userdata, float time, void* output)
{
    
}
