//
// Created by Amanjeet Singh on 18/09/20.
//
#include <jni.h>
#include "jni_interface.h"
#include "android_debug.h"

extern "C" JNIEXPORT jstring JNICALL Java_com_developers_perfmon_MainActivity_getSysInfo(JNIEnv *env, jobject) {
    LOGD("Hello from C++ world");
    return (*env).NewStringUTF("Hello from C");
}

