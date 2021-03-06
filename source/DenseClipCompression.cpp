#include "DenseClipCompression.h"
#include <climits>
#include "assert.h"
#include <algorithm>
#include <cmath>

const float kBiggestFloatSmallerThanOne = 0.99999994f;
inline int CeilfToInt(float f)
{
    assert(f >= INT_MIN && f <= INT_MAX);
    return f >= 0 ? (int)(f + kBiggestFloatSmallerThanOne) : (int)(f);
}

int DenseClipCompressBegin(const void* builder, int extractedCurveCount, int curveIterCount, float beginTime, float endTime, float sampleRate, const int customValue, char** retBlobData)
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
    
    IUnityAnimationCompression* customAnimation = GetUnityInterfaces().Get<IUnityAnimationCompression>();
    
    float* evaluateValue = new float[4];
    for (int i = 0; i < curveIterCount; i++)
    {
        for (int j = 0; j < frameCount; j++)
        {
            float time = beginTime + ((float)j / sampleRate);
            const AnimationCurveForPlugin* curveData = customAnimation->GetCurve(builder, i);
            if (curveData == nullptr)
                continue;
            
            customAnimation->OnCurveEvaluateClamp(builder, i, time, evaluateValue);
            
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
            
            int curIndex = (curveData->extractedCurveIndex + j * extractedCurveCount) * floatSize + headerIndex;
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

void DenseClipCompressEnd(char** retBlobData)
{
    if (*retBlobData != nullptr)
        delete[] *retBlobData;
    *retBlobData = nullptr;
}

void* OnDenseClipLoad(const char* blobData, size_t blobSize, const int customValue)
{
    int intSize = sizeof(int);
    int floatSize = sizeof(float);
    DenseClipUserData* userData = new DenseClipUserData();
    userData->headerIndex = 0;
    userData->frameCount = *reinterpret_cast<const int*>(blobData + userData->headerIndex);
    userData->headerIndex += intSize;
    userData->extractedCurveCount = *reinterpret_cast<const int*>(blobData + userData->headerIndex);
    userData->headerIndex += intSize;
    userData->sampleRate = *reinterpret_cast<const float*>(blobData + userData->headerIndex);
    userData->headerIndex += floatSize;
    userData->beginTime = *reinterpret_cast<const float*>(blobData + userData->headerIndex);
    userData->headerIndex += floatSize;
    userData->blobData = reinterpret_cast<const char*>(blobData + userData->headerIndex);
    return userData;
}

void OnDesneClipUnload(void* userData)
{
    if (userData == nullptr)
        return;
    DenseClipUserData* denseClipUserData = reinterpret_cast<DenseClipUserData*>(userData);
    delete denseClipUserData;
}

inline float MathLerp(float a, float b, float x)
{
    return x * (b - a) + a;
}

void BlendArray(const float* lhs, const float* rhs, size_t size, float t, float* output)
{
    for (size_t i = 0; i < size; i++)
        output[i] = MathLerp(lhs[i], rhs[i], t);
}

void  PrepareBlendValues(const DenseClipUserData* denseClipUserData, float time, float*& lhs, float*& rhs, float& u)
{
    time -= denseClipUserData->beginTime;

    float index;
    u = modf(time * denseClipUserData->sampleRate, &index);

    int lhsIndex = (int)index;
    int rhsIndex = lhsIndex + 1;
    lhsIndex = std::max(0, lhsIndex);
    lhsIndex = std::min(denseClipUserData->frameCount - 1, lhsIndex);

    rhsIndex = std::max(0, rhsIndex);
    rhsIndex = std::min(denseClipUserData->frameCount - 1, rhsIndex);
    const float* floatBlobData = reinterpret_cast<const float*>(denseClipUserData->blobData);

    lhs = const_cast<float*>(&floatBlobData[lhsIndex * denseClipUserData->extractedCurveCount]);
    rhs = const_cast<float*>(&floatBlobData[rhsIndex * denseClipUserData->extractedCurveCount]);
}

void OnDesneClipSample(void* userData, float time, float* output)
{
    if (userData == nullptr)
        return;
    
    DenseClipUserData* denseClipUserData = reinterpret_cast<DenseClipUserData*>(userData);
    float u;
    float* lhsPtr;
    float* rhsPtr;
    PrepareBlendValues(denseClipUserData, time, lhsPtr, rhsPtr, u);

    BlendArray(lhsPtr, rhsPtr, denseClipUserData->extractedCurveCount, u, output);
}
