#ifndef _THREAD_H_
#define _THREAD_H_

#include <stdint.h>
#include <stddef.h>
#include <cpu.h>

#define THREAD_NAME_MAX_LEN 32
#define THREAD_MAX_ID (uint16_t-1)

#define THREAD_DEFAULT_STACK_SIZE   0x10000

typedef enum {
    NEW, //Not sure if needed
    INIT,
    RUN,
    READY,
    SLEEP,
    WAIT,
    DEAD
} thread_status;

typedef struct thread_t thread_t;

#include <task.h>

struct thread_t {
    uint16_t tid;
    char thread_name[THREAD_NAME_MAX_LEN];
    uintptr_t stack;
    struct task_t* parent_task;
    thread_status status;
    size_t ticks;
    cpu_status_t *execution_frame;
    size_t wakeup_time;
    thread_t* next_sibling;
    thread_t* next;
    uint64_t* rsp0;
};


extern size_t next_thread_id;


thread_t* create_thread(char* thread_name, void (*_entry_point)(void *) , void* arg, struct task_t* parent_task);
void thread_execution_wrapper( void (*)(void *), void*);
void thread_suicide_trap();
void thread_sleep(size_t millis);
void thread_wakeup(thread_t* thread);
char *get_thread_status(thread_t *thread);
// Functions below are for tests purposes
void noop(void *c);
void noop2(void *c);
void noop3(void *c);

#endif
