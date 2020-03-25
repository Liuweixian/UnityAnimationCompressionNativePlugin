#include "DenseClipCompression.h"
#include <climits>
#include "assert.h"
#include <algorithm>

static IUnityInterfaces* s_UnityInterfaces = NULL;
IUnityInterfaces& GetUnityInterfaces() { return *s_UnityInterfaces; }
static IUnityAnimationCompression* s_CustomAnimation = NULL;

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces)
{
    s_UnityInterfaces = unityInterfaces;
    s_CustomAnimation = s_UnityInterfaces->Get<IUnityAnimationCompression>();
    s_CustomAnimation->OnClipCompress = DenseClipCompress;
    s_CustomAnimation->OnClipLoad = OnDenseClipLoad;
    s_CustomAnimation->OnClipUnload = OnDesneClipUnload;
    s_CustomAnimation->OnClipSample = OnDesneClipSample;
}

const float kBiggestFloatSmallerThanOne = 0.99999994f;
inline int CeilfToInt(float f)
{
    assert(f >= INT_MIN && f <= INT_MAX);
    return f >= 0 ? (int)(f + kBiggestFloatSmallerThanOne) : (int)(f);
}

int DenseClipCompress(const void* builder, int curveCount, float startTime, float stopTime, float sampleRate, char* retBlobData)
{
    int intSize = sizeof(int);
    int floatSize = sizeof(float);
    int frameCount = std::max<int>(CeilfToInt(stopTime - startTime) * sampleRate + 1, 2);
    int totalSize = 2 * intSize + (frameCount + 2) * floatSize;
    retBlobData = new char[totalSize];
    int index = 0;
    retBlobData[index] = frameCount;
    index += intSize;
    retBlobData[index] = curveCount;
    index += intSize;
    retBlobData[index] = sampleRate;
    index += floatSize;
    retBlobData[index] = startTime;
    
    float* evaluateValue = new float[4];
    for (int i = 0; i < curveCount; i++)
    {
        for (int j = 0; j < frameCount; j++)
        {
            float time = startTime + ((float)j / sampleRate);
            const AnimationCurveForPlugin* curveData = s_CustomAnimation->GetCurve(builder, i);
            if (curveData == nullptr)
                continue;
            
            s_CustomAnimation->OnCurveEvaluateClamp(builder, i, time, evaluateValue);
            
            int valueCount = 0;
            switch (curveData->keyframeType) {
                case AnimationCurveForPlugin::kKeyframeFloat:
                    valueCount = 1;
                    break;
                case AnimationCurveForPlugin::kKeyframeVector3:
                    valueCount = 3;
                    break;
                case AnimationCurveForPlugin::kKeyframeQuaternion:
                    valueCount = 4;
                    break;
                default:
                    break;
            }
            
            for (int k = 0; k < valueCount; k++)
            {
                index += floatSize;
                retBlobData[index] = evaluateValue[k];
            }
        }
    }
    delete[] evaluateValue;
    return totalSize;
}

void* OnDenseClipLoad(void* blobData, size_t blobSize)
{
    return nullptr;
}

void OnDesneClipUnload(void* userData)
{
    
}

void OnDesneClipSample(void* userData, float* output)
{
    
}
