#pragma once
#include "IUnityAnimationCompression.h"

void*   OnDenseClipLoad(void* blobData, size_t blobSize); //return userData
void    OnDesneClipUnload(void* userData);
void    OnDesneClipSample(void* userData, float* output);

int     DenseClipCompress(const void* builder, int extractedCurveCount, int curveIterCount, float beginTime, float endTime, float sampleRate, char** retBlobData);
