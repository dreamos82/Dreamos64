#include <thread.h>
#include <scheduler.h>
#include <stdio.h>
#include <video.h>
#include <string.h>
#include <kheap.h>

thread_t* create_thread(char* thread_name, int (*_entry_point)(void *data)) {
    thread_t *new_thread = kmalloc(sizeof(thread_t));
    new_thread->tid = next_thread_id++;
    new_thread->status = INIT;
    strcpy(new_thread->thread_name, thread_name);
    new_thread->execution_frame = kmalloc(sizeof(cpu_status_t));
    new_thread->execution_frame->interrupt_number = 0x101;
    (new_thread->execution_frame)->error_code = 0x0;
    //TODO Intialize cpu_status_t
    return new_thread;
}

void noop() {
    asm("nop");
}
