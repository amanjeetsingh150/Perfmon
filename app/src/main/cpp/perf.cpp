//
// Created by Amanjeet Singh on 18/09/20.
//
#include <jni.h>
#include "jni_interface.h"
#include <sys/sysinfo.h>
#include <sys/resource.h>
#include "android_debug.h"

static constexpr auto kMillisInSec = 1000;
static constexpr auto kMicrosInMillis = 1000;

inline uint64_t timeval_sum_to_millis(timeval &tv1, timeval &tv2) {
    return (tv1.tv_sec + tv2.tv_sec) * kMillisInSec +
           (tv1.tv_usec + tv2.tv_usec) / kMicrosInMillis;
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_developers_perfmon_MainActivity_getCpuTime(JNIEnv *env, jobject) {
    struct sysinfo si{};
    rusage rusageStats{};

    sysinfo (&si);

    getrusage(RUSAGE_THREAD, &rusageStats);
    uint64_t cpu_time_millis = timeval_sum_to_millis(
            rusageStats.ru_utime,
            rusageStats.ru_stime
    );

    return cpu_time_millis;
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_developers_perfmon_DetailsActivity_getCpuTime(JNIEnv *env, jobject) {
    rusage rusageStats{};

    getrusage(RUSAGE_THREAD, &rusageStats);
    uint64_t cpu_time_millis = timeval_sum_to_millis(
            rusageStats.ru_utime,
            rusageStats.ru_stime
    );

    return cpu_time_millis;
}


