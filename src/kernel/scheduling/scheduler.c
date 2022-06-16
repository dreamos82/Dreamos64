#include <scheduler.h>
#include <framebuffer.h>
#include <string.h>
#include <stdio.h>
#include <logging.h>
#include <kheap.h>

uint16_t scheduler_ticks;
size_t next_thread_id;
size_t next_thread_index;

thread_t* thread_list;
thread_t* current_executing_thread;
thread_t* current_executing_thread;
thread_t* idle_thread;

size_t thread_list_size;

void init_scheduler() {
    scheduler_ticks = 0;
    next_thread_index = 0; 
    next_thread_id = 0;
    current_executing_thread = NULL;
    thread_list = NULL;
    idle_thread = NULL;
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
            //loglinef(Verbose, "Current uptime: %d - wakeup: %d", get_kernel_uptime(), current_thread->wakeup_time);
            if ( get_kernel_uptime() > current_thread->wakeup_time) {
                current_thread->status = READY;
                thread_to_execute = current_thread;
                break;
            }
        }
        
        if (current_thread->status == DEAD) {
            scheduler_delete_thread(current_thread->tid);
        } else if (current_thread->status == READY || current_thread->status == NEW) {
            thread_to_execute = current_thread;
            break;
        }
        prev_thread_tid = current_thread->tid;
        current_thread = scheduler_get_next_thread();        
    }

    //loglinef(Verbose, "Picked thread: %d - %s list size: %d", thread_to_execute->tid, thread_to_execute->thread_name, thread_list_size);
    thread_to_execute->status = RUN;
    thread_to_execute->ticks = 0;
    current_executing_thread = thread_to_execute;
    return current_executing_thread->execution_frame;
}


/*cpu_status_t* schedule(cpu_status_t* cur_status) {
    // The scheduling function take as parameter the current iret_frame cur_status, and if is time to change task (ticks threshold reached)
    // It save it to the current tax, and select a new one for execution and return the new task execution frame.
    // If the tick threshold has not been reached it return cur_status as it is
    thread_t *current_thread = current_executing_thread;

    thread_t* prev_thread = current_executing_thread;

    if (current_executing_thread->status == SLEEP) {
        loglinef(Verbose, "Thread %d, is sleeping", current_executing_thread->tid);
        scheduler_ticks = SCHEDULER_NUMBER_OF_TICKS;
    }

    if (scheduler_ticks != SCHEDULER_NUMBER_OF_TICKS) {
        return cur_status;
    }
   // We first reset the ticks for the next task
   scheduler_ticks = 0;
   if (thread_list_size != 0) {
        loglinef(Verbose, "prev_thread status: %d", prev_thread->status);
        if (prev_thread->status != NEW) {
            prev_thread->execution_frame = cur_status;
        }
        
        if (prev_thread->status == DEAD) {
            scheduler_delete_thread(prev_thread->tid);
        } else if(prev_thread->status != SLEEP) {
            prev_thread->status = READY;
        }
        current_executing_thread = scheduler_get_next_thread();       
        while (current_executing_thread->status == SLEEP) {
            loglinef(Verbose, "current_executing_thread->wakeup_time: %d - current_uptime: %d", current_executing_thread->wakeup_time, get_kernel_uptime());
            if(!(get_kernel_uptime() > current_executing_thread->wakeup_time)) {
                current_executing_thread = scheduler_get_next_thread();
            } else {
                break;
            }
        }
        
        loglinef(Verbose, "- new_thread is: %d, old thread is: %d - status: %d", current_executing_thread->tid, prev_thread->tid, current_executing_thread->status);
        if (current_executing_thread != NULL && prev_thread->tid != current_executing_thread->tid) {
            loglinef(Verbose, "Picked task: %d, name: %s - prev_thread tid: %d", current_executing_thread->tid, current_executing_thread->thread_name, prev_thread->tid);
            current_executing_thread->status = RUN;
            return current_executing_thread->execution_frame;
        }
   }
   return cur_status;
}*/

void scheduler_add_thread(thread_t* thread) {
    thread->next = thread_list;    
    thread_list_size++;
    thread_list = thread;
    loglinef(Verbose, "Adding thread: %s - %d", thread_list->thread_name, thread_list->tid);
    if (current_executing_thread == NULL) {
        //This means that there are no tasks on the queue yet.
        current_executing_thread = thread;
        loglinef(Verbose, "Selected thread is: %d", current_executing_thread->tid);
    }
}

void scheduler_delete_thread(size_t thread_id) {
    loglinef(Verbose, "Called with thread id: %d", thread_id);
    thread_t *thread_item = thread_list;
    thread_t *prev_item = NULL;
    
    // First thing: we should search for the task to be deleted.
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
    logline(Verbose, "Interrupting current_thread");
    asm("int $0x20");
}

