#ifndef _LOG_H
#define _LOG_H

#include <stddef.h>
#include <stdbool.h>

#define LOG_DEBUG 0
#define LOG_VERBOSE 1
#define LOG_INFO 2
#define LOG_ERROR 3
//fatal will trap the kernel in an infinite loop, kind of like panic() would do.
#define LOG_FATAL 4

typedef enum {
    Debug = 0,
    Verbose = 1,
    Info = 2,
    Error = 3,
    Fatal = 4,
} log_level_t;

#define pretty_logf(level, msg, ...)  loglinef(level, "(%s): "msg,  __FUNCTION__, __VA_ARGS__);
#define pretty_log(level, msg)  loglinef(level, "(%s): "msg,  __FUNCTION__);

#define LOG_OUTPUT_DONT_CARE (size_t)-1
#define LOG_OUTPUT_SERIAL (1 << 0)
#define LOG_OUTPUT_DEBUGCON (1 << 1)
#define LOG_OUTPUT_FRAMEBUFFER (1 << 2)
#define LOG_OUTPUT_COUNT 3

void init_log(size_t defaultOutputs, log_level_t trimBelowLevel, bool useVgaVideo);
void set_log_trim_level(size_t newTrim);
void logline(log_level_t level, const char* msg);
void loglinef(log_level_t level, const char* msg, ...);

#endif
