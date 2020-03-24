#include "IUnityAnimationCompression.h"
#include <climits>
#include "assert.h"
#include <algorithm>
void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityIssueAnimationCompressionFunc(UnityAnimationCompressionFuncs* callbacks)
{
    callbacks->OnCurveCompress = OnCurveCustomCompress;
    callbacks->OnCurveLoad = OnCurveCustomLoad;
    callbacks->OnCurveUnload = OnCurveCustomUnload;
    callbacks->OnCurveSample = OnCurveCustomSample;
}

const float kBiggestFloatSmallerThanOne = 0.99999994f;
inline int CeilfToInt(float f)
{
    assert(f >= INT_MIN && f <= INT_MAX);
    return f >= 0 ? (int)(f + kBiggestFloatSmallerThanOne) : (int)(f);
}

int OnCurveCustomCompress(AnimationCurveForPlugin* allCurves, int curveCount, float startTime, float stopTime, float sampleRate, void *output)
{
    int frameCount = std::max<int>(CeilfToInt(stopTime - startTime) * sampleRate + 1, 2);
    for (int i = 0; i < curveCount; i++)
    {
        AnimationCurveForPlugin curCurve = allCurves[i];
        for (int j = 0; j < frameCount; j++)
        {
            float time = startTime + ((float)j / sampleRate);
            float* retValue = nullptr;
            curCurve.Evaluate(time, retValue);
        }
    }
    return frameCount * curveCount;
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
