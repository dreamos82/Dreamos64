#ifndef _THREAD_H_
#define _THREAD_H_

#include <stdint.h>
#include <stddef.h>
#include <cpu.h>

#define TASK_NAME_MAX_LEN 32
#define THREAD_NAME_MAX_LEN 32
#define THREAD_MAX_ID (uint16_t-1)
#define THREAD_TICKS 0x50

#define THREAD_DEFAULT_STACK_SIZE   0x10000

typedef enum {
    NEW, //Not sure if needed
    INIT,
    RUN,
    READY,
    WAIT,
    DEAD
} thread_status;

typedef struct {
    size_t task_id;
    char task_name[TASK_NAME_MAX_LEN];
    struct thread_t* threads;
    //TODO: add list of child threads
} task_t;

struct thread_t {
    uint16_t tid;
    char thread_name[THREAD_NAME_MAX_LEN];

    uint64_t *stack;
    
    task_t* parent;
    thread_status status;
    size_t ticks;
    cpu_status_t *execution_frame;
    struct thread_t* next;
};

typedef struct thread_t thread_t;
extern size_t next_thread_id;

thread_t* create_thread(char*, void (*)(void *), void*);
void thread_execution_wrapper( void (*)(void *), void*);
void thread_suicide_trap();
void noop();
#endif
