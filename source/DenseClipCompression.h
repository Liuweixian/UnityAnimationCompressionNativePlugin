#pragma once
#include "IUnityAnimationCompression.h"

struct DenseClipUserData
{
    int         frameCount;
    int         extractedCurveCount;
    float       sampleRate;
    float       beginTime;
    int         headerIndex;
    const char* blobData;
};

void*   OnDenseClipLoad(const char* blobData, size_t blobSize); //return userData
void    OnDesneClipUnload(void* userData);
void    OnDesneClipSample(void* userData, float time, float* output);

int     DenseClipCompressBegin(const void* builder, int extractedCurveCount, int curveIterCount, float beginTime, float endTime, float sampleRate, char** retBlobData);
void    DenseClipCompressEnd(char** retBlobData);
