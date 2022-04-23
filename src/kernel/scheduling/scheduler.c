#include <scheduler.h>
#include <framebuffer.h>
#include <string.h>

uint8_t scheduler_ticks;
thread_t thread_pool[5];
size_t next_thread_id;
void init_scheduler() {
    scheduler_ticks = 0;
    next_thread_id = 0;
    
    for(int i=0; i < 5; i++) {
        thread_pool[i].tid = 0;
        thread_pool[i].status = INIT;
    }
}

void schedule() {
   #if USE_FRAMEBUFFER == 1
        if(scheduler_ticks ==  SCHEDULER_NUMBER_OF_TICKS) {
        scheduler_ticks = 0;
        _fb_printStr("*", 0, 11, 0x000000, 0xE169CD);
        scheduler_ticks = 0;
    } else {
        _fb_printStr("/", 0, 11, 0x000000, 0xE169CD);
    }
    #endif
}
