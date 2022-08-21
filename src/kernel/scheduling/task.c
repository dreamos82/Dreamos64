#include <task.h>
#include <scheduler.h>
#include <kheap.h>
#include <string.h>
#include <logging.h>


task_t* create_task(char *name, void (*_entry_point)(void *), void *args) {
    task_t* new_task = (task_t*) kmalloc(sizeof(task_t));
    strcpy(new_task->task_name, name);
    new_task->parent = NULL;
    new_task->task_id = next_task_id++;
    loglinef(Verbose, "Task created with name: %s - Task id: %d", new_task->task_name, new_task->task_id);
    thread_t* thread = create_thread(name, _entry_point, args, new_task);
    new_task->threads = thread;
    scheduler_add_task(new_task);
    return new_task;
}

bool add_thread_to_task_by_id(size_t task_id, thread_t* thread) {
    task_t* task = get_task(task_id);
    if (task_id > next_task_id || task == NULL) {
        return false;
    }
    thread->next = task->threads;
    thread->next_sibling = thread;
    return true;
}

bool add_thread_to_task(task_t* task, thread_t* thread) {
    if (task == NULL) {
        return false;
    }
    thread->next = task->threads;
    thread->next_sibling = thread;
    return true;
}

task_t* get_task(size_t task_id) {
    if (task_id > next_task_id) {
        return NULL;
    }
    task_t* cur_task = root_task;
    while ( cur_task != NULL ) {
        loglinef(Verbose, "Searching task: %d", cur_task->task_id);
        if ( cur_task->task_id == task_id ) {
            return cur_task;
        }
        cur_task = cur_task->next;
    }
    return NULL;
}

void print_thread_list(size_t task_id) {
    task_t* task = get_task(task_id);
    if (task != NULL) {
        thread_t* thread = task->threads;
        while(thread != NULL) {
            loglinef(Verbose, "\tThread; %d - %s", thread->tid, thread->thread_name);
            thread = thread->next;
        }
    }
}
