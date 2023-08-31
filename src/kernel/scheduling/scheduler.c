#include <scheduler.h>
#include <framebuffer.h>
#include <string.h>
#include <stdio.h>
#include <logging.h>
#include <kheap.h>
#include <kernel.h>
#include <vm.h>
#include <task.h>

uint16_t scheduler_ticks;
size_t next_thread_id;
size_t next_task_id;

thread_t* thread_list;
thread_t* current_executing_thread;
thread_t* idle_thread;
task_t* root_task;

size_t thread_list_size;

void init_scheduler() {
    scheduler_ticks = 0;
    next_task_id = 0;
    next_thread_id = 0;
    current_executing_thread = NULL;
    thread_list = NULL;
    idle_thread = NULL;
    root_task = NULL;
    thread_list_size = 0;
}

cpu_status_t* schedule(cpu_status_t* cur_status) {
    // The scheduling function take as parameter the current iret_frame cur_status, and if is time to change task (ticks threshold reached)
    // It save it to the current task, and select a new one for execution and return the new task execution frame.
    // If the tick threshold has not been reached it return cur_status as it is

    thread_t* current_thread = current_executing_thread;
    thread_t* prev_executing_thread;
    thread_t* thread_to_execute = idle_thread;
    uint16_t prev_thread_tid = -1;
    //loglinef(Verbose, "Cur thread: %u %s", current_thread->tid, current_thread->thread_name);
    //loglinef(Verbose, "(schedule) ---Cur stack: 0x%x", cur_status->rsp);
    // First let's check if the current task need to be scheduled or not;
    if (current_executing_thread->status == SLEEP) {
        // If the task has been placed to sleep it needs to be scheduled
        //loglinef(Verbose, "Current thread %d status is sleeping!", current_executing_thread->tid);
        current_executing_thread->ticks = SCHEDULER_NUMBER_OF_TICKS;
    }

    if (current_executing_thread->ticks++ < SCHEDULER_NUMBER_OF_TICKS) {
        return cur_status;
    }

    // We don't want to change the execution frame of a newly creted task
    if (current_executing_thread->status != NEW) {
        current_executing_thread->execution_frame = cur_status;
    }

    if (current_executing_thread->status != SLEEP && current_executing_thread->status != DEAD) {
        current_executing_thread->status = READY;
    }

    prev_executing_thread = current_executing_thread;
    prev_thread_tid = prev_executing_thread->tid;
    current_thread = scheduler_get_next_thread();

    while (current_thread->tid != prev_thread_tid) {
        if (current_thread->status == SLEEP) {
            loglinef(Verbose, "(schedule) This thread %d is sleeping", current_thread->tid);
            //loglinef(Verbose, "Current uptime: %d - wakeup: %d", get_kernel_uptime(), current_thread->wakeup_time);
            if ( get_kernel_uptime() > current_thread->wakeup_time) {
                //loglinef(Verbose, "(schedule) --->WAKING UP: %d - thread_name: %s", current_thread->tid, current_thread->thread_name);
                current_thread->status = READY;
                thread_to_execute = current_thread;
                break;
            }
        }

        if (current_thread->status == DEAD) {
            remove_thread_from_task(current_thread->tid, current_thread->parent_task);
            scheduler_delete_thread(current_thread->tid);
        } else if (current_thread->status == READY || current_thread->status == NEW) {
            thread_to_execute = current_thread;
            break;
        }
        prev_thread_tid = current_thread->tid;
        current_thread = scheduler_get_next_thread();
    }

    thread_to_execute->status = RUN;
    thread_to_execute->ticks = 0;
    current_executing_thread = thread_to_execute;
    task_t *current_task = current_executing_thread->parent_task;
    load_cr3(current_task->vm_root_page_table);
    //loglinef(Verbose, "(schedule) leaving schedule...");
    return current_executing_thread->execution_frame;
}

void scheduler_add_task(task_t* task) {
    if (root_task == NULL) {
        loglinef(Verbose, "(scheduler_add_task) First task being added: %s", task->task_name);
    }
    task->next = root_task;
    root_task = task;
}

void scheduler_add_thread(thread_t* thread) {
    thread->next = thread_list;
    thread_list_size++;
    thread_list = thread;
    loglinef(Verbose, "(scheduler_add_thread) Adding thread: %s - %d", thread_list->thread_name, thread_list->tid);
    if (current_executing_thread == NULL) {
        //This means that there are no tasks on the queue yet.
        current_executing_thread = thread;
    }
}

void scheduler_delete_thread(size_t thread_id) {
    loglinef(Verbose, "(scheduler_delete_thread) Called with thread id: %d", thread_id);
    thread_t *thread_item = thread_list;
    thread_t *prev_item = NULL;

    // First thing: we should search for the thread to be deleted.
    while (thread_item != NULL && thread_item->tid != thread_id ) {
        prev_item = thread_item;
        thread_item = thread_item->next;
    }

    if (thread_item == NULL) {
        return;
    }

    kfree(thread_item->execution_frame);
    kfree(thread_item->stack - THREAD_DEFAULT_STACK_SIZE);

    if (thread_item == thread_list) {
        // If thread_item == thread_list it means that it is the first item so we just need
        // to make the root of the stack to point to the next item
        thread_list = thread_list->next;
        thread_list_size--;
    } else {
        // Otherwise we only need to make the previous thread
        // to point to thread pointe by the one we are deleting
        prev_item->next = thread_item->next;
        thread_list_size--;
    }
    kfree(thread_item);
}

thread_t* scheduler_get_next_thread() {
    if (thread_list_size == 0 || thread_list == NULL) {
        return NULL;
    }

    if (current_executing_thread == NULL) {
        return thread_list;
    }
    if (current_executing_thread->next == NULL) {
        return thread_list;
    }
    return current_executing_thread->next;
}

size_t scheduler_get_queue_size() {
    thread_t *thread = thread_list;
    uint32_t counter = 0;
    while (thread != NULL) {
        counter++;
        thread = thread->next;
    }
    return counter;
}

void scheduler_yield() {
    logline(Verbose, "(scheulder_yield) Interrupting current_thread");
    asm("int $0x20");
}

