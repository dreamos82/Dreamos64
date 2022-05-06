#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <stdint.h>
#include <thread.h>

#define SCHEDULER_NUMBER_OF_TICKS   0x200

//extern thread_t thread_pool[5]; //This is temporary
void init_scheduler();

void schedule();
extern uint16_t scheduler_ticks;
#endif
