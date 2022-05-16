#include <thread.h>
#include <scheduler.h>
#include <stdio.h>
#include <video.h>
#include <string.h>
#include <kheap.h>
#include <logging.h>

thread_t* create_thread(char* thread_name, void (*_entry_point)(void *), void* arg) {
    thread_t *new_thread = kmalloc(sizeof(thread_t));
    new_thread->tid = next_thread_id++;
    new_thread->status = NEW;
    strcpy(new_thread->thread_name, thread_name);
    new_thread->next = NULL;
    new_thread->ticks = 0;
    loglinef(Verbose, "Creating thread with arg: %c - arg: %x", (char) *((char*) arg), &arg);
    new_thread->execution_frame = kmalloc(sizeof(cpu_status_t));
    new_thread->execution_frame->interrupt_number = 0x101;
    new_thread->execution_frame->error_code = 0x0;
    new_thread->execution_frame->rip = thread_execution_wrapper;
    new_thread->execution_frame->rdi = _entry_point;
    new_thread->execution_frame->rsi = arg;
    new_thread->execution_frame->rflags = 0x202;
    new_thread->execution_frame->ss = 0x10;
    new_thread->execution_frame->cs = 0x8;
    // We need to allocate a new stack for each thread
    uint64_t *stack_pointer = kmalloc(THREAD_DEFAULT_STACK_SIZE);
    
    // The stack grow backward, so the pointer will be the end of the stack
    new_thread->stack = stack_pointer + THREAD_DEFAULT_STACK_SIZE;
    new_thread->execution_frame->rsp = new_thread->stack;
    
    scheduler_add_thread(new_thread);
    //TODO Intialize cpu_status_t
    return new_thread;
}

void thread_suicide_trap() {
    logline(Verbose, "Suicide function called");
}

void thread_execution_wrapper( void (*_thread_function)(void *), void* arg) {
    _thread_function(arg);
    thread_suicide_trap();
    return;
}

void noop(char *c) {
    int i=0;
    char *str[2];
    str[0] = *c;
    str[1] = '\0';
    while(1) {
        i++;
        loglinef(Verbose, "Task: %c - %d", (char) *c, i);
        #if USE_FRAMEBUFFER == 1
        _fb_printStr(str, 0, 12, 0x000000, 0xE169CD);
        #endif
    }
    asm("nop");
}

void noop2(char *c) {
    int i=0;
    char *str[2];
    str[0] = *c;
    str[1] = '\0';

    while(1) {
        i++;
        loglinef(Verbose, "Task2: %c - %d", (char) *c, i);
        #if USE_FRAMEBUFFER == 1
        _fb_printStr(str, 0, 12, 0x000000, 0xE169CD);
        #endif

    }
    asm("nop");
}
