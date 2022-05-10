#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <stdint.h>
#include <thread.h>

#define SCHEDULER_NUMBER_OF_TICKS   0x200
#define SCHEDULER_MAX_THREAD_NUMBER 0x10

extern uint16_t scheduler_ticks;

void init_scheduler();
void schedule();
void scheduler_add_thread(thread_t*);
thread_t* scheduler_get_next_thread();
size_t scheduler_get_queue_size();
#endif
