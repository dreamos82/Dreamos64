#ifndef _THREAD_H_
#define _THREAD_H_

#include <stdint.h>
#include <stddef.h>

#define THREAD_NAME_MAX_LEN 32
#define THREAD_MAX_ID (uint8_t-1)

typedef enum {
    INIT,
    RUN,
    WAIT,
    DEAD
} thread_status;

typedef struct {
    uint16_t tid;
    char thread_name[THREAD_NAME_MAX_LEN];
    thread_status status;
} thread_t;

extern size_t next_thread_id;

thread_t create_thread(char*);

#endif
