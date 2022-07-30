#ifndef _TASK_H
#define _TASK_H

#include <stddef.h>
#include <thread.h>

#define TASK_NAME_MAX_LEN 32

typedef struct task_t task_t;

struct task_t {
    size_t task_id;
    char task_name[TASK_NAME_MAX_LEN];
    struct thread_t* threads;
    task_t* parent;
    //TODO: add list of child threads
};

#endif
