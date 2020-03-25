#pragma once
#include "IUnityInterface.h"
#include <stdlib.h>

struct AnimationCurveForPlugin
{
    enum KeyframeType
    {
        kInvalid = 0,
        kKeyframeVector3 = 1,
        kKeyframeQuaternion = 2,
        kKeyframeFloat = 3,
    };
    KeyframeType keyframeType;
    int keyframeCount;
    const void* unityCurvePointer;
};

struct KeyFrameForPlugin
{
    float time;
    float value[4];
    float inSlope[4];
    float outSlope[4];
};

IUnityInterfaces& GetUnityInterfaces();
#define ANIMATION_COMPRESSION_FUNCS_CALL(funcname, ...)                                                     \
    IUnityAnimationCompression* pluginCallback = GetUnityInterfaces().Get<IUnityAnimationCompression>();    \
    if (pluginCallback != NULL && pluginCallback->funcname != NULL)                                         \
        pluginCallback->funcname(__VA_ARGS__);                                                              \

#define ANIMATION_COMPRESSION_FUNCS_CALL_RET(retval, funcname, ...)                                         \
    IUnityAnimationCompression* pluginCallback = GetUnityInterfaces().Get<IUnityAnimationCompression>();    \
    if (pluginCallback != NULL && pluginCallback->funcname != NULL)                                         \
        retval = pluginCallback->funcname(__VA_ARGS__);                                                     \

UNITY_DECLARE_INTERFACE(IUnityAnimationCompression)
{
    //runtime api, need implement in plugin
    void*   (*OnClipLoad)(void* blobData, size_t blobSize); //return userData
    void    (*OnClipUnload)(void* userData);
    void    (*OnClipSample)(void* userData, float* output);
    
    //editor api for compression need
    //interface for plugin, need implement in plugin for customize compression
    int     (*OnClipCompress)(const void* builder, int curveCount, float startTime, float stopTime, float sampleRate, char* retBlobData);
    //interface from unity, utility function for helping do compression, remeber no needed to implement in plugin
    void  (*OnCurveEvaluate)(const void* builder, int curveIndex, float time, float* retValue);
    void  (*OnCurveEvaluateClamp)(const void* builder, int curveIndex, float time, float* retValue);
    void  (*GetCurveKeyFrame)(const void* builder, int curveIndex, int frameIndex, KeyFrameForPlugin* retValue);
    const AnimationCurveForPlugin* (*GetCurve)(const void* builder, int curveIndex);
};
UNITY_REGISTER_INTERFACE_GUID(0x5655381C42484721ULL, 0xA06479CEF39BF9E9ULL, IUnityAnimationCompression)


