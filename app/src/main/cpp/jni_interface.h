//
// Created by Amanjeet Singh on 18/09/20.
//

#include <jni.h>

#ifndef PERFMON_JNI_INTERFACE_H
#define PERFMON_JNI_INTERFACE_H


extern "C" JNIEXPORT jstring JNICALL Java_com_developers_perfmon_MainActivity_getSysInfo(JNIEnv *env, jobject);


#endif //PERFMON_JNI_INTERFACE_H
