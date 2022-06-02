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
thread_t* selected_thread;
size_t thread_list_size;

void init_scheduler() {
    scheduler_ticks = 0;
    next_thread_index = 0; 
    next_thread_id = 0;
    selected_thread = NULL;
    thread_list = NULL;
    thread_list_size = 0;
}


cpu_status_t* schedule(cpu_status_t* cur_status) {
    // The scheduling function take as parameter the current iret_frame cur_status, and if is time to change task (ticks threshold reached)
    // It save it to the current tax, and select a new one for execution and return the new task execution frame.
    // If the tick threshold has not been reached it return cur_status as it is
    thread_t* prev_thread = selected_thread;

    if (selected_thread->status == SLEEP) {
        loglinef(Verbose, "Thread %d, is sleeping", selected_thread->tid);
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
        selected_thread = scheduler_get_next_thread();       
        while (selected_thread->status == SLEEP) {
            loglinef(Verbose, "selected_thread->wakeup_time: %d - current_uptime: %d", selected_thread->wakeup_time, get_kernel_uptime());
            if(!(get_kernel_uptime() > selected_thread->wakeup_time)) {
                selected_thread = scheduler_get_next_thread();
            } else {
                break;
            }
        }
        
        loglinef(Verbose, "- new_thread is: %d, old thread is: %d - status: %d", selected_thread->tid, prev_thread->tid, selected_thread->status);
        if (selected_thread != NULL && prev_thread->tid != selected_thread->tid) {
            loglinef(Verbose, "Picked task: %d, name: %s - prev_thread tid: %d", selected_thread->tid, selected_thread->thread_name, prev_thread->tid);
            selected_thread->status = RUN;
            return selected_thread->execution_frame;
        }
   }
   return cur_status;
}

void scheduler_add_thread(thread_t* thread) {
    thread->next = thread_list;    
    thread_list_size++;
    thread_list = thread;
    loglinef(Verbose, "Adding thread: %s - %d", thread_list->thread_name, thread_list->tid);
    if (selected_thread == NULL) {
        //This means that there are no tasks on the queue yet.
        selected_thread = thread;
        loglinef(Verbose, "Selected thread is: %d", selected_thread->tid);
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
    kfree(thread_item->stack);
    
    if (thread_item == thread_list) {
        // If thread_item == thread_list it means that it is the first item so we just need 
        // to make the root of the stack to point to the next item
        thread_list = thread_list->next;
        thread_list_size--;
    } else {
        // Otherwise we only need to make the previous thread
        // to point to thread pointe by the one we are deleting
        prev_item->next = thread_list->next;
        thread_list_size--;
    }
    kfree(thread_item);
}

thread_t* scheduler_get_next_thread() {
    if (thread_list_size == 0 || thread_list == NULL) {
        return NULL;
    }
    if (selected_thread == NULL) {
        return thread_list;
    }
    if (selected_thread->next == NULL) {
        return thread_list;
    }
    return selected_thread->next;    
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

