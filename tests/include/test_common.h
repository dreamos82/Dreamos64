#ifndef _TEST_COMMON_H
#define _TEST_COMMON_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define pretty_assert(msg, expected_value, returned_value) \
    ({ \
        printf("\t[%s] %s Returned value: %d should be:  %d\n", __FUNCTION__, msg, expected_value, returned_value); \
        assert(expected_value == returned_value); \
    })

typedef enum {
    Debug = 0,
    Verbose = 1,
    Info = 2,
    Error = 3,
    Fatal = 4,
} log_level_t;

typedef struct {
    bool    locked;
} spinlock_t;


void spinlock_acquire(spinlock_t *lock);
void spinlock_release(spinlock_t *lock);
void spinlock_free(spinlock_t *lock);

void _printStringAndNumber(char *, unsigned long);
void _printStr(const char *);
void _printNewLine();
void init_log(size_t defaultOutputs, log_level_t trimBelowLevel, bool useVgaVideo);
void set_log_trim_level(size_t newTrim);
void logline(log_level_t level, const char* msg);
void loglinef(log_level_t level, const char* msg, ...);
void *map_phys_to_virt_addr(void* physical_address, void* address, unsigned int flags);

uint32_t _compute_kernel_entries(uint64_t);
#endif
