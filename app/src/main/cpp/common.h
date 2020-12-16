//
// Created by Amanjeet Singh on 16/12/20.
//


#ifndef PERFMON_COMMON_H
#define PERFMON_COMMON_H

#include <cstdint>

// Custom parse for unsinged long values, ignores minus sign and skips blank
// spaces in front. Such narrowly specialized method is faster than the standard
// strtoull.
uint64_t parse_all(char* str, char** end);

int32_t systemClockTickIntervalMs();

#endif //PERFMON_COMMON_H
