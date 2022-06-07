#include <thread.h>
#include <scheduler.h>
#include <stdio.h>
#include <framebuffer.h>
#include <string.h>
#include <kheap.h>
#include <logging.h>
#include <kernel.h>

thread_t* create_thread(char* thread_name, void (*_entry_point)(void *), void* arg) {
    thread_t *new_thread = kmalloc(sizeof(thread_t));
    new_thread->tid = next_thread_id++;
    new_thread->parent = NULL;
    new_thread->status = NEW;
    new_thread->wakeup_time = 0;
    strcpy(new_thread->thread_name, thread_name);
    new_thread->next = NULL;
    new_thread->ticks = 0;
    loglinef(Verbose, "Creating thread with arg: %c - arg: %x - name: %s", (char) *((char*) arg), arg, thread_name);
    //Here we create a new execution frame to be used when switching to a newly created task
    new_thread->execution_frame = kmalloc(sizeof(cpu_status_t));
    new_thread->execution_frame->interrupt_number = 0x101;
    new_thread->execution_frame->error_code = 0x0;
    new_thread->execution_frame->rip = (uint64_t) thread_execution_wrapper;
    new_thread->execution_frame->rdi = (uint64_t) _entry_point;
    new_thread->execution_frame->rsi = (uint64_t) arg;
    new_thread->execution_frame->rflags = 0x202;
    new_thread->execution_frame->ss = 0x10;
    new_thread->execution_frame->cs = 0x8;
    // We need to allocate a new stack for each thread
    uint64_t *stack_pointer = kmalloc(THREAD_DEFAULT_STACK_SIZE);
    
    // The stack grow backward, so the pointer will be the end of the stack
    new_thread->stack = stack_pointer + THREAD_DEFAULT_STACK_SIZE;
    new_thread->execution_frame->rsp = (uint64_t) new_thread->stack;
    
    scheduler_add_thread(new_thread);
    return new_thread;
}

void thread_sleep(size_t millis) {
    current_executing_thread->status = SLEEP;
    uint64_t kernel_uptime = get_kernel_uptime();
    current_executing_thread->wakeup_time = kernel_uptime + millis; // To change with millis since boot + millis
    loglinef(Verbose, "Kernel uptime is: %u - wakeup time is: %u", kernel_uptime, current_executing_thread->wakeup_time);
    scheduler_yield();
}

void thread_wakeup(thread_t* thread) {

}

void thread_suicide_trap() {
    loglinef(Verbose, "Suicide function called on thread: %d", current_executing_thread->tid);
    current_executing_thread->status = DEAD;
    while(1);
}

void thread_execution_wrapper( void (*_thread_function)(void *), void* arg) {
    _thread_function(arg);
    thread_suicide_trap();
    return;
}

void noop(char *c) {
    int i=0;
    char str[2];
    str[0] = *c;
    str[1] = '\0';
    while(1) {
        i++;
        //loglinef(Verbose, "Task: %c - %d", (char) *c, i);
        #if USE_FRAMEBUFFER == 1
        _fb_printStr(str, 0, 12, 0x000000, 0xE169CD);
        #endif
    }
    asm("nop");
}

void noop2(char *c) {
    int i=0;
    char str[2];
    str[0] = *c;
    str[1] = '\0';
    //thread_sleep(5000);
    while(i < 100) {
        //i++;
        //loglinef(Verbose, "Task2: %c - %d", (char) *c, i);
        #if USE_FRAMEBUFFER == 1
        _fb_printStr(str, 0, 12, 0x000000, 0xE169CD);
        #endif

    }
}

void noop3(char *c) {
    int i=0;
    char str[4];
    str[0] = *c;
    str[1] = 'b';
    str[2] = 'b';
    str[3] = '\0';
    while(i < 100) {
        i++;
        //loglinef(Verbose, "Task2: %c - %d", (char) *c, i);
        #if USE_FRAMEBUFFER == 1
        _fb_printStr(str, 0, 12, 0x000000, 0xE169CD);
        #endif
    }
    loglinef(Verbose, "Going to sleep %d", get_kernel_uptime());
    thread_sleep(5000);
    loglinef(Verbose, "Wakeup %d - %d", get_kernel_uptime(), current_executing_thread->wakeup_time);
    while(i < 100) {
        i++;
        //loglinef(Verbose, "Task2: %c - %d", (char) *c, i);
        #if USE_FRAMEBUFFER == 1
        _fb_printStr("r", 0, 12, 0x000000, 0xE169CD);
        #endif

    }
}
