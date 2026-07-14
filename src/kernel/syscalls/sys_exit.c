#include <logging.h>
#include <sys_exit.h>
#include <scheduler.h>
#include <task.h>

void sys_exit(int status) {
    //task_t *current_task = current_executing_thread->parent_task;
    //current_task->exit_code = status;
    //current_task->status = EXTERMINATED;
    // Since the kernel schedule threads there is no status for the task
    // I need to terminate every single thread manually.
    // By setting their status to dead
    return;
}

void sys_thread_exit(int status) {
    current_executing_thread->exit_code = status;
    current_executing_thread->status = DEAD;
    current_executing_thread->ticks = SCHEDULER_NUMBER_OF_TICKS;
    pretty_logf(Info, "Thread exiting with status: %d", status);
    // An exited thread shouldn't exit, otherwise it can run into garbage.
    while(1);
}
