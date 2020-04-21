//
//  Entry.cpp
//  AnimationCompression
//
//  Created by Liu WeiXian on 30/3/2020.
//  Copyright © 2020 Liu WeiXian. All rights reserved.
//
#include "DenseClipCompression.h"

static IUnityInterfaces* s_UnityInterfaces = NULL;
IUnityInterfaces& GetUnityInterfaces() { return *s_UnityInterfaces; }

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces)
{
    s_UnityInterfaces = unityInterfaces;
    IUnityAnimationCompression* customAnimation = s_UnityInterfaces->Get<IUnityAnimationCompression>();
    customAnimation->OnClipCompressBegin = DenseClipCompressBegin;
    customAnimation->OnClipCompressEnd = DenseClipCompressEnd;
    customAnimation->OnClipLoad = OnDenseClipLoad;
    customAnimation->OnClipUnload = OnDesneClipUnload;
    customAnimation->OnClipSample = OnDesneClipSample;
}
