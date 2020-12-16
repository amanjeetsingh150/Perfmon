//
// Created by Amanjeet Singh on 18/09/20.
//

#include <jni.h>

#ifndef PERFMON_JNI_INTERFACE_H
#define PERFMON_JNI_INTERFACE_H


extern "C" JNIEXPORT jlong JNICALL
Java_com_developers_perfmon_MainActivity_getCpuTime(JNIEnv *env, jobject);

#endif //PERFMON_JNI_INTERFACE_H
extern "C"
JNIEXPORT jlong JNICALL
Java_com_developers_perfmon_DetailsActivity_getCpuTime(JNIEnv *env, jobject thiz);

extern "C"
JNIEXPORT jlong JNICALL
Java_com_developers_perfmon_MainActivity_getCpuTimeFromProc(
        JNIEnv *env,
        jobject thiz,
        jlong thread_id
);

extern "C"
JNIEXPORT jlong JNICALL
Java_com_developers_perfmon_DetailsActivity_getCpuTimeFromProc(
        JNIEnv *env,
        jobject thiz,
        jlong thread_id
);