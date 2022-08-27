#ifndef _TASK_H
#define _TASK_H

#include <stddef.h>
#include <stdbool.h>
#include <thread.h>

#define TASK_NAME_MAX_LEN 32

typedef struct task_t task_t;

struct task_t {
    size_t task_id;
    char task_name[TASK_NAME_MAX_LEN];
    struct thread_t* threads;
    task_t* parent;
    //TODO: add list of child threads
    task_t* next;
};

extern size_t next_task_id;

task_t* create_task(char *name, void (*_entry_point)(void *), void *args);
task_t* get_task(size_t task_id);

bool add_thread_to_task_by_id(size_t task_id, thread_t* thread);
bool add_thread_to_task(task_t* task, thread_t* thread);
void print_thread_list(size_t task_id);
#endif
