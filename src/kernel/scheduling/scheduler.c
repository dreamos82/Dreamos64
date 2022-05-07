#include <scheduler.h>
#include <framebuffer.h>
#include <string.h>

uint16_t scheduler_ticks;
thread_t *thread_pool[5];
size_t next_thread_id;
size_t cur_thread_index;
size_t next_thread_index;

void init_scheduler() {
    scheduler_ticks = 0;
    cur_thread_index = 0;
    next_thread_index = 0; 
    next_thread_id = 0;
    
    for(int i=0; i < 5; i++) {
        thread_pool[i] = NULL;
        //thread_pool[i].status = INIT;
    }
}

void schedule() {
   #if USE_FRAMEBUFFER == 1
   if(scheduler_ticks ==  SCHEDULER_NUMBER_OF_TICKS) {
        scheduler_ticks = 0;
        cur_thread_index = (cur_thread_index + 1) % 5;
        _fb_printStrAndNumber("i:", cur_thread_index, 0, 12, 0x000000, 0xE169CD);
        if(cur_thread_index < next_thread_id) {
            printf("Picked task: %d - name: %s\n", thread_pool[cur_thread_index]->tid, thread_pool[cur_thread_index]->thread_name);
        }
        //_fb_printStr("*", 0, 11, 0x000000, 0xE169CD);
    } else {
        //_fb_printStr("/", 0, 11, 0x000000, 0xE169CD);
    }
    #endif
}

void scheduler_add_thred(thread_t* thread) {
   if(next_thread_index < SCHEDULER_MAX_THREAD_NUMBER) {
        thread_pool[next_thread_index++] = thread;
        printf("Adding thread: %s - %d\n", thread->thread_name, next_thread_index);
    } 
}
