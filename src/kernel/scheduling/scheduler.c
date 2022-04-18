#include <scheduler.h>
#include <framebuffer.h>

uint8_t scheduler_ticks;
void init_scheduler() {
    scheduler_ticks = 0;
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
