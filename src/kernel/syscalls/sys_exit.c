#include <logging.h>
#include <sys_exit.h>
#include <scheduler.h>
#include <task.h>

void sys_exit(int status) {
    // Since the kernel schedule threads there is no status for the task
    // I need to terminate every single thread manually.
    // By setting their status to dead
    pretty_log(Info, "Placeholder for sys_exit for task, not implemented yet");
    return;
}

void sys_thread_exit(int status) {
    current_executing_thread->exit_code = status;
    current_executing_thread->status = DEAD;
    current_executing_thread->ticks = SCHEDULER_NUMBER_OF_TICKS;
    pretty_logf(Info, "Thread exiting with status: %d", status);
}
