//
// Created by Amanjeet Singh on 15/12/20.
//

#include "ProcFs.h"
#include "unistd.h"
#include "fcntl.h"
#include "system_error"
#include "android_debug.h"

static constexpr int kMaxProcFileLength = 64;

TaskStatInfo::TaskStatInfo()
        : cpuTime(0),
          state(ThreadState::TS_UNKNOWN),
          majorFaults(0),
          cpuNum(-1),
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

TaskStatInfo parseStatFile(char *data, size_t size) {
    const char *end = (data + size);

    // TODO: parse stat file
    LOGD("Hello from C++ %s", data);

    return TaskStatInfo();
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
