//
// Created by Amanjeet Singh on 16/12/20.
//

#include <cstdint>
#include <unistd.h>
#include <algorithm>

uint64_t parse_all(char* str, char** end) {
    static constexpr int kMaxDigits = 20;

    char* cur = str;
    while (*cur == ' ') {
        ++cur;
    }

    uint64_t result = 0;
    uint8_t len = 0;
    while (*cur >= '0' && *cur <= '9' && len <= kMaxDigits) {
        result *= 10;
        result += (*cur - '0');
        ++len;
        ++cur;
    }

    *end = cur;
    return result;
}

int32_t systemClockTickIntervalMs() {
    int clockTick = static_cast<int32_t>(sysconf(_SC_CLK_TCK));
    if (clockTick <= 0) {
        return 0;
    }
    return std::max(1000 / clockTick, 1);
}
