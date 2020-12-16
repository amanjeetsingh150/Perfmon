//
// Created by Amanjeet Singh on 15/12/20.
//

#include "ProcFs.h"
#include "unistd.h"
#include "fcntl.h"
#include "system_error"
#include "android_debug.h"
#include "common.h"

static constexpr int kMaxProcFileLength = 64;

TaskStatInfo::TaskStatInfo()
        : cpuTime(0),
          state(ThreadState::TS_UNKNOWN),
          majorFaults(0),
          kernelCpuTimeMs(0),
          minorFaults(0),
          threadPriority(999) {}

/**
 * Consumes data until `ch` or we reach `end`.
 *
 * @return Returns a pointer immediately after `ch`.
 */
char *skipUntil(char *data, const char *end, char ch) {
    // It's important that we check against `end`
    // before we dereference `data`.
    while (data < end && *data != ch) {
        if (*data == '\0') {
            throw std::runtime_error("Unexpected end of string");
        }

        ++data;
    }

    if (data == end) {
        throw std::runtime_error("Unexpected end of string");
    }

    // One past the `ch` character.
    return ++data;
}

inline ThreadState convertCharToStateEnum(char stateChar) {
    switch (stateChar) {
        case 'R':
            return TS_RUNNING;
        case 'S':
            return TS_SLEEPING;
        case 'D':
            return TS_WAITING;
        case 'Z':
            return TS_ZOMBIE;
        case 'T':
            return TS_STOPPED;
        case 't':
            return TS_TRACING_STOP;
        case 'X':
        case 'x':
            return TS_DEAD;
        case 'K':
            return TS_WAKEKILL;
        case 'W':
            return TS_WAKING;
        case 'P':
            return TS_PARKED;
        default:
            return TS_UNKNOWN;
    }
}

TaskStatInfo parseStatFile(char *data, size_t size) {
    const char *end = (data + size);

    data = skipUntil(data, end, ' '); // pid
    data = skipUntil(data, end, ')'); // name
    data = skipUntil(data, end, ' '); // space after name
    char state = *data; // state

    data = skipUntil(data, end, ' '); // state
    data = skipUntil(data, end, ' '); // ppid
    data = skipUntil(data, end, ' '); // pgrp
    data = skipUntil(data, end, ' '); // session
    data = skipUntil(data, end, ' '); // tty_nr
    data = skipUntil(data, end, ' '); // tpgid

    data = skipUntil(data, end, ' '); // flags

    char *endptr = nullptr;
    auto minflt = parse_all(data, &endptr); // minflt

    if (errno == ERANGE || data == endptr || endptr > end) {
        throw std::runtime_error("Could not parse minflt");
    }

    data = skipUntil(endptr, end, ' ');

    data = skipUntil(data, end, ' '); // cminflt

    endptr = nullptr;
    auto majflt = parse_all(data, &endptr); // majflt
    if (errno == ERANGE || data == endptr || endptr > end) {
        throw std::runtime_error("Could not parse majflt");
    }
    data = skipUntil(endptr, end, ' ');

    data = skipUntil(data, end, ' '); // cmajflt

    endptr = nullptr;
    auto utime = parse_all(data, &endptr); // utime
    if (errno == ERANGE || data == endptr || endptr > end) {
        throw std::runtime_error("Could not parse utime");
    }
    data = skipUntil(endptr, end, ' ');

    endptr = nullptr;
    auto stime = parse_all(data, &endptr); // stime
    if (errno == ERANGE || data == endptr || endptr > end) {
        throw std::runtime_error("Could not parse stime");
    }
    data = skipUntil(endptr, end, ' ');

    data = skipUntil(data, end, ' '); // cutime
    data = skipUntil(data, end, ' '); // cstime

    endptr = nullptr;
    auto priority = strtol(data, &endptr, 10); // priority
    if (errno == ERANGE || data == endptr || endptr > end) {
        throw std::runtime_error("Could not parse priority");
    }

    // SYSTEM_CLK_TCK is defined as 100 in linux as is unchanged in android.
    // Therefore there are 10 milli seconds in each clock tick.
    static int kClockTicksMs = systemClockTickIntervalMs();

    TaskStatInfo info{};
    info.cpuTime = kClockTicksMs * (utime + stime);
    info.kernelCpuTimeMs = kClockTicksMs * stime;
    info.state = convertCharToStateEnum(state);
    info.majorFaults = majflt;
    info.minorFaults = minflt;
    info.threadPriority = priority;

    return info;
}

std::string tidToStatPath(int32_t tid, const char *stat_name) {
    char threadStatPath[kMaxProcFileLength]{};

    int bytesWritten = snprintf(
            threadStatPath,
            kMaxProcFileLength,
            "/proc/self/task/%d/%s",
            tid,
            stat_name);

    if (bytesWritten < 0 || bytesWritten >= kMaxProcFileLength) {
        throw std::system_error(
                errno, std::system_category(), "Could not format file path");
    }
    return std::string(threadStatPath);
}

TaskStatInfo ProcFs::readStatFile() {
    const std::string &path = tidToStatPath(threadId, "stat");
    fd = openStatFile(path);

    // Seek file to beginning
    if (lseek(fd, 0, SEEK_SET)) {
        throw std::system_error(
                errno, std::system_category(), "Could not rewind file");
    }

    // Read proc stat file
    constexpr size_t bufferLength = 512;
    char buffer[bufferLength]{};
    int bytes_read = read(fd, buffer, (sizeof(buffer) - 1));
    if (bytes_read < 0) {
        throw std::system_error(
                errno,
                std::system_category(),
                "Could not read stat file"
        );
    }

    return parseStatFile(buffer, bytes_read);
}

int ProcFs::openStatFile(const std::string &path) {

    int statFile = open(path.c_str(), O_SYNC | O_RDONLY);

    // Could'nt open stat file from proc
    if (statFile == -1) {
        throw std::system_error(
                errno, std::system_category(), "Could not open stat file");
    }
    return statFile;
}
