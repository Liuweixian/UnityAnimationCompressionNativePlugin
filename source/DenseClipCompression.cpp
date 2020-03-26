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

int DenseClipCompress(const void* builder, int extractedCurveCount, int curveIterCount, float beginTime, float endTime, float sampleRate, char** retBlobData)
{
    int intSize = sizeof(int);
    int floatSize = sizeof(float);
    int frameCount = std::max<int>(CeilfToInt((endTime - beginTime) * sampleRate) + 1, 2);
    int totalSize = 2 * intSize + (2 + frameCount * extractedCurveCount) * floatSize;
    *retBlobData = new char[totalSize];
    memset(*retBlobData, 0, totalSize);
    
    int headerIndex = 0;
    *reinterpret_cast<int*>(*retBlobData + headerIndex) = frameCount;
    headerIndex += intSize;
    *reinterpret_cast<int*>(*retBlobData + headerIndex) = extractedCurveCount;
    headerIndex += intSize;
    *reinterpret_cast<float*>(*retBlobData + headerIndex) = sampleRate;
    headerIndex += floatSize;
    *reinterpret_cast<float*>(*retBlobData + headerIndex) = beginTime;
    headerIndex += floatSize;
    
    float* evaluateValue = new float[4];
    for (int i = 0; i < curveIterCount; i++)
    {
        for (int j = 0; j < frameCount; j++)
        {
            float time = beginTime + ((float)j / sampleRate);
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
            
            int curIndex = j * extractedCurveCount + curveData->extractedCurveIndex * floatSize + headerIndex;
            for (int k = 0; k < valueCount; k++)
            {
                float retValue = evaluateValue[k];
                *reinterpret_cast<float*>(*retBlobData + curIndex) = retValue;
                curIndex += floatSize;
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
