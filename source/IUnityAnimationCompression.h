#pragma once
#include <stdlib.h>

#if defined(__CYGWIN32__)
    #define UNITY_INTERFACE_API __stdcall
    #define UNITY_INTERFACE_EXPORT __declspec(dllexport)
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(WINAPI_FAMILY)
    #define UNITY_INTERFACE_API __stdcall
    #define UNITY_INTERFACE_EXPORT __declspec(dllexport)
#elif defined(__MACH__) || defined(__ANDROID__) || defined(__linux__)
    #define UNITY_INTERFACE_API
    #define UNITY_INTERFACE_EXPORT
#else
    #define UNITY_INTERFACE_API
    #define UNITY_INTERFACE_EXPORT
#endif

class AnimationCurveForPlugin
{
public:
    // PPtrKeyframe	is not supported
    enum KeyFrameType 
    {
        kInvalid = 0,
        kKeyframeVector3 = 1,
        kKeyframeQuaternion = 2,
        kKeyframeFloat = 3,
    };

    AnimationCurveForPlugin(void* unityCurvePointer, KeyFrameType keyframeType);
    AnimationCurveForPlugin();
    void Evaluate(float time, float* output);

private:
	void* m_unityCurvePointer;
	KeyFrameType m_KeyFrameType;
};

struct UnityAnimationCompressionFuncs
{
    // returns number of bytes generated
    // output points to blob data
    int (*OnCurveCompress)(AnimationCurveForPlugin* curve, int curveCount, void* output);
    // pass in a blob generated by OnCurveCompress and return a userdata
    void* (*OnCurveLoad)(void* blob, size_t blobSize);
    void (*OnCurveUnload)(void* userdata);
    void (*OnCurveSample)(void* userdata, float time, void* output);
};

int OnCurveCustomCompress(AnimationCurveForPlugin* curve, int curveCount, void* output);
void* OnCurveCustomLoad(void* blob, size_t blobSize);
void OnCurveCustomUnload(void* userdata);
void OnCurveCustomSample(void* userdata, float time, void* output);

#ifdef __cplusplus
extern "C" {
#endif
// If exported by a plugin, this function will be called for setup animation compression functions
void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityIssueAnimationCompressionFunc(UnityAnimationCompressionFuncs* callbacks);

#ifdef __cplusplus
}
#endif
