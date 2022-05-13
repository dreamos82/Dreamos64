#include <scheduler.h>
#include <framebuffer.h>
#include <string.h>
#include <stdio.h>
#include <logging.h>

uint16_t scheduler_ticks;
size_t next_thread_id;
size_t cur_thread_index;
size_t next_thread_index;

thread_t* thread_list;
thread_t* selected_thread;
size_t thread_list_size;

void init_scheduler() {
    scheduler_ticks = 0;
    cur_thread_index = 0;
    next_thread_index = 0; 
    next_thread_id = 0;
    selected_thread = NULL;
    thread_list = NULL;
    thread_list_size = 0;
}

cpu_status_t* schedule(cpu_status_t* cur_status) {
   #if USE_FRAMEBUFFER == 1
   if(scheduler_ticks ==  SCHEDULER_NUMBER_OF_TICKS) {
        scheduler_ticks = 0;
        cur_thread_index = (cur_thread_index + 1) % 5;
        _fb_printStrAndNumber("i:", cur_thread_index, 0, 12, 0x000000, 0xE169CD);        
        if(thread_list_size != 0) {
            //selected_thread->status = READY;
            selected_thread = scheduler_get_next_thread();
            if(selected_thread != NULL) {
                loglinef(Verbose, "Picked task: %d, name: %s", selected_thread->tid, selected_thread->thread_name);
                selected_thread->status = RUN;
            }
        }
        //if(cur_thread_index < next_thread_id) {
            //printf("Picked task: %d - name: %s\n", thread_pool[cur_thread_index]->tid, thread_pool[cur_thread_index]->thread_name);
        //}
        //_fb_printStr("*", 0, 11, 0x000000, 0xE169CD);
    } else {
        //_fb_printStr("/", 0, 11, 0x000000, 0xE169CD);
    }
    #endif

}

void scheduler_add_thread(thread_t* thread) {
    thread->next = thread_list;    
    thread_list_size++;
    thread_list = thread;
    loglinef(Verbose, "Adding thread: %s - %d", thread_list->thread_name, thread_list->tid);
    if (selected_thread == NULL) {
        //This means that there are not tasks on the queue yet.
        selected_thread = thread;
    }
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
    return 0;
}
