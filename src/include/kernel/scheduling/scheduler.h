#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <stdint.h>
#include <thread.h>
#include <task.h>
#include <cpu.h>

#define SCHEDULER_NUMBER_OF_TICKS   0x200
#define SCHEDULER_MAX_THREAD_NUMBER 0x10

extern uint16_t scheduler_ticks;
extern thread_t* current_executing_thread;
extern thread_t* idle_thread;
extern task_t* current_executing_task;

void init_scheduler();
cpu_status_t* schedule(cpu_status_t* cur_status);

void scheduler_add_thread(thread_t* thread);
void scheduler_add_task(task_t* task);

thread_t* scheduler_get_next_thread();

size_t scheduler_get_queue_size();
void scheduler_delete_thread(size_t tid);
void scheduler_yield();
#endif
