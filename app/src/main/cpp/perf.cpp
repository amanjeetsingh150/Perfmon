//
// Created by Amanjeet Singh on 18/09/20.
//
#include <jni.h>
#include "jni_interface.h"
#include <sys/sysinfo.h>
#include <sys/resource.h>
#include "android_debug.h"
#include "ProcFs.h"

static constexpr auto kMillisInSec = 1000;
static constexpr auto kMicrosInMillis = 1000;

jlong get_cpu_time_from_rusage();

jlong get_cpu_time_from_proc(jlong thread_id);

inline uint64_t timeval_sum_to_millis(timeval &tv1, timeval &tv2) {
    return (tv1.tv_sec + tv2.tv_sec) * kMillisInSec +
           (tv1.tv_usec + tv2.tv_usec) / kMicrosInMillis;
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_developers_perfmon_MainActivity_getCpuTime(JNIEnv *env, jobject) {
    return get_cpu_time_from_rusage();
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_developers_perfmon_DetailsActivity_getCpuTime(JNIEnv *env, jobject) {
    return get_cpu_time_from_rusage();
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_developers_perfmon_MainActivity_getCpuTimeFromProc(JNIEnv *env, jobject, jlong thread_id) {
    return get_cpu_time_from_proc(thread_id);
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_developers_perfmon_DetailsActivity_getCpuTimeFromProc(JNIEnv *env, jobject, jlong thread_id) {
    return get_cpu_time_from_proc(thread_id);
}

jlong get_cpu_time_from_proc(jlong thread_id) {
    auto *pFs = new ProcFs(thread_id);
    const TaskStatInfo &info = pFs->readStatFile();
    return info.cpuTime;
}

jlong get_cpu_time_from_rusage() {
    rusage rusageStats{};

    getrusage(RUSAGE_THREAD, &rusageStats);
    uint64_t cpu_time_millis = timeval_sum_to_millis(
            rusageStats.ru_utime,
            rusageStats.ru_stime
    );

    return cpu_time_millis;
}

