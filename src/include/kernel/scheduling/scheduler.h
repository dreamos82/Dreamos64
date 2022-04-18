#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <stdint.h>

#define SCHEDULER_NUMBER_OF_TICKS   0x30
void init_scheduler();

void schedule();

extern uint8_t scheduler_ticks;
#endif
