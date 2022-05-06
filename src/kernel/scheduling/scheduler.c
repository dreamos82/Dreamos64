#include <scheduler.h>
#include <framebuffer.h>
#include <string.h>

uint16_t scheduler_ticks;
thread_t thread_pool[5];
size_t next_thread_id;
size_t cur_thread_index;
void init_scheduler() {
    scheduler_ticks = 0;
    cur_thread_index = 0;
    next_thread_id = 0;
    
    for(int i=0; i < 5; i++) {
        thread_pool[i].tid = 0;
        thread_pool[i].status = INIT;
    }
}

void schedule() {
   #if USE_FRAMEBUFFER == 1
   printf("%d \n", cur_thread_index);
   if(scheduler_ticks ==  SCHEDULER_NUMBER_OF_TICKS) {
        scheduler_ticks = 0;
        printf("Called");
        cur_thread_index = (cur_thread_index + 1) % 5;
        _fb_printStrAndNumber("i:", cur_thread_index, 0, 12, 0x000000, 0xE169CD);
        //_fb_printStr("*", 0, 11, 0x000000, 0xE169CD);
    } else {
        printf("sch: %d\n", scheduler_ticks);
        //_fb_printStr("/", 0, 11, 0x000000, 0xE169CD);
    }
    #endif
}
