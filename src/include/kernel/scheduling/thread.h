#ifndef _THREAD_H_
#define _THREAD_H_

#include <stdint.h>
#include <stddef.h>
#include <cpu.h>

#define THREAD_NAME_MAX_LEN 32
#define THREAD_MAX_ID (uint16_t-1)

typedef enum {
    INIT,
    RUN,
    WAIT,
    DEAD
} thread_status;

typedef struct {
    size_t task_id;
    //TODO: add list of child threads
} task_t;

typedef struct {
    uint16_t tid;
    char thread_name[THREAD_NAME_MAX_LEN];
    task_t* parent;
    thread_status status;
    cpu_status_t execution_frame;
} thread_t;

extern size_t next_thread_id;

thread_t* create_thread(char*);

#endif
