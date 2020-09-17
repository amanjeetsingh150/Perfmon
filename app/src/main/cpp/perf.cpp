//
// Created by Amanjeet Singh on 18/09/20.
//
#include <jni.h>
#include "jni_interface.h"
#include <sys/sysinfo.h>
#include "android_debug.h"

extern "C" JNIEXPORT jstring JNICALL Java_com_developers_perfmon_MainActivity_getSysInfo(JNIEnv *env, jobject) {
    LOGD("Hello from C++ world");
    struct sysinfo si{};
    sysinfo (&si);
    LOGD("Hello from C++ world with %d", si.procs);
    return (*env).NewStringUTF("Hello from C");
}

