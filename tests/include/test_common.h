#ifndef _TEST_COMMON_H
#define _TEST_COMMON_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
typedef enum {
    Debug = 0,
    Verbose = 1,
    Info = 2,
    Error = 3,
    Fatal = 4,
} log_level_t;

void _printStringAndNumber(char *, unsigned long);
void _printStr(const char *);
void _printNewLine();
void init_log(size_t defaultOutputs, log_level_t trimBelowLevel, bool useVgaVideo);
void set_log_trim_level(size_t newTrim);
void logline(log_level_t level, const char* msg);
void loglinef(log_level_t level, const char* msg, ...);

uint32_t _compute_kernel_entries(uint64_t);
#endif
