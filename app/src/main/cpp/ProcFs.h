//
// Created by Amanjeet Singh on 15/12/20.
//


#ifndef PERFMON_PROCFS_H
#define PERFMON_PROCFS_H


#include <cstdint>
#include "unistd.h"
#include "string"

enum ThreadState : int {
    TS_RUNNING = 1, // R
    TS_SLEEPING = 2, // S
    TS_WAITING = 3, // D
    TS_ZOMBIE = 4, // Z
    TS_STOPPED = 5, // T
    TS_TRACING_STOP = 6, // t
    TS_PAGING = 7, // W
    TS_DEAD = 8, // X, x
    TS_WAKEKILL = 9, // K
    TS_WAKING = 10, // W
    TS_PARKED = 11, // P

    TS_UNKNOWN = 0,
};

struct TaskStatInfo {
    uint64_t cpuTime;
    ThreadState state;
    uint64_t majorFaults;
    uint8_t cpuNum;
    uint64_t kernelCpuTimeMs;
    uint64_t minorFaults;
    int16_t threadPriority;

    TaskStatInfo();
};

class ProcFs {

public:
    explicit ProcFs() = delete ;
    explicit ProcFs(int32_t threadId) : threadId(threadId) {}

    ~ProcFs() {
        if (fd != -1) {
            close(fd);
            fd = -1;
        }
    }

    TaskStatInfo readStatFile();
    static int openStatFile(const std::string &path);

private:
    int32_t threadId;
    int fd = -1;
};


#endif //PERFMON_PROCFS_H
