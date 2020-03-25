#pragma once
#include "IUnityAnimationCompression.h"

void*   OnDenseClipLoad(void* blobData, size_t blobSize); //return userData
void    OnDesneClipUnload(void* userData);
void    OnDesneClipSample(void* userData, float* output);

int     DenseClipCompress(const void* builder, int curveCount, float startTime, float stopTime, float sampleRate, char* retBlobData);
