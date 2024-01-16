#include <thread.h>
#include <scheduler.h>
#include <stdio.h>
#include <framebuffer.h>
#include <string.h>
#include <kheap.h>
#include <logging.h>
#include <kernel.h>
#include <userspace.h>
#include <vmm.h>
#include <vmm_mapping.h>

unsigned char code_to_run[] = {
    0xeb, 0xfe
};

thread_t* create_thread(char* thread_name, void (*_entry_point)(void *), void* arg, task_t* parent_task, bool is_supervisor) {
    // The first part is pretty trivial mostly bureaucray. Setting basic thread information like name, tid, parent...
    // Just like when registtering a new born child :D
    if ( parent_task == NULL) {
        pretty_log(Fatal, "Cannot create thread without parent task");
    }

    thread_t *new_thread = kmalloc(sizeof(thread_t));
    new_thread->tid = next_thread_id++;
    new_thread->parent_task = parent_task;
    new_thread->status = NEW;
    new_thread->wakeup_time = 0;
    strcpy(new_thread->thread_name, thread_name);
    new_thread->next = NULL;
    new_thread->next_sibling = NULL;
    new_thread->ticks = 0;
    pretty_logf(Verbose, "Creating thread with arg: %c - arg: %x - name: %s - rip: %x", (char) *((char*) arg), arg, thread_name, _entry_point);

    //Here we create a new execution frame to be used when switching to a newly created task
    new_thread->execution_frame = kmalloc(sizeof(cpu_status_t));
    new_thread->execution_frame->interrupt_number = 0x101;
    new_thread->execution_frame->error_code = 0x0;
    if (!is_supervisor) {
        // This piece of code is temporary, just to test a userspace task, it run just an infinite loop.
        pretty_logf(Verbose, "vmm_data address: 0x%x", &(parent_task->vmm_data));
        new_thread->execution_frame->rip = prepare_userspace_function(&(parent_task->vmm_data));
        pretty_logf(Verbose, "using userspace function address: 0x%x", new_thread->execution_frame->rip);
        new_thread->execution_frame->rdi = 0;
        new_thread->execution_frame->rsi = 0;
    } else {
        pretty_logf(Verbose, "using idle function: 0x%x", _entry_point);
        new_thread->execution_frame->rip = (uint64_t) code_to_run;
        //new_thread->execution_frame->rip = (uint64_t) thread_execution_wrapper;
        //new_thread->execution_frame->rdi = (uint64_t) _entry_point;
        //new_thread->execution_frame->rsi = (uint64_t) arg;
    }
    // rdi and rsi are the two arguments passed to the thread_execution_wrapper function
    new_thread->execution_frame->rflags = 0x202;
    if ( is_supervisor ) {
        new_thread->execution_frame->ss = 0x10;
        new_thread->execution_frame->cs = 0x08;
    } else {
        // For user mode cs is 1b and ss is 23
        new_thread->execution_frame->ss = 0x23;
        new_thread->execution_frame->cs = 0x1B;
    }

    // Every thread need it's kernel stack allocated (aka rsp0 field of the TSS)
    new_thread->rsp0 = kmalloc(THREAD_DEFAULT_STACK_SIZE) + THREAD_DEFAULT_STACK_SIZE;
    if (new_thread->rsp0 == NULL) {
          pretty_log(Fatal, "rsp0 is null - PANIC!");
          while(1);
    }
    // We need to allocate a new stack for each thread
    //void* stack_pointer = kmalloc(THREAD_DEFAULT_STACK_SIZE);
    void* stack_pointer = vmm_alloc(THREAD_DEFAULT_STACK_SIZE, VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE | VMM_FLAGS_STACK, &(parent_task->vmm_data));
    if (stack_pointer == NULL) {
        pretty_log(Fatal, "rsp is null - PANIC!");
        while(1);
    }

    // The stack grow backward, so the pointer will be the end of the stack
    new_thread->stack = (uintptr_t)stack_pointer;
    new_thread->execution_frame->rsp = (uint64_t) new_thread->stack;
    new_thread->execution_frame->rbp = 0;
    pretty_logf(Verbose, "thread: %s stack address returned: 0x%x", new_thread->thread_name, new_thread->execution_frame->rsp);
    if (parent_task != NULL) {
        add_thread_to_task(parent_task, new_thread);
    } else {
        pretty_log(Fatal, "Cannot create thread without parent task");
    }

    scheduler_add_thread(new_thread);
    return new_thread;
}

void thread_sleep(size_t millis) {
    current_executing_thread->status = SLEEP;
    uint64_t kernel_uptime = get_kernel_uptime();
    current_executing_thread->wakeup_time = kernel_uptime + millis; // To change with millis since boot + millis
    pretty_logf(Verbose, "(thread_sleep) Kernel uptime is: %u - wakeup time is: %u", kernel_uptime, current_executing_thread->wakeup_time);
    scheduler_yield();
}

void thread_wakeup(thread_t* thread) {
(void)thread;
}

void thread_suicide_trap() {
    current_executing_thread->status = DEAD;
    pretty_logf(Verbose, "(thread_suicide_trap) Suicide function called on thread: %d name: %s - Status: %s", current_executing_thread->tid, current_executing_thread->thread_name, get_thread_status(current_executing_thread));
    while(1);
}

void thread_execution_wrapper( void (*_thread_function)(void *), void* arg) {
    pretty_logf(Verbose, "Calling function %x",  _thread_function);
    _thread_function(arg);
    thread_suicide_trap();
    return;
}

void idle(void *c) {
    while(1);
}

void noop(void *v) {
    asm("nop");
}

char *get_thread_status(thread_t *thread) {
    switch(thread->status) {
        case NEW:
            return "NEW";
        case INIT:
            return "INIT";
        case RUN:
            return "RUN";
        case READY:
            return "READY";
        case SLEEP:
            return "SLEEP";
        case WAIT:
            return "WAIT";
        case DEAD:
            return "DEAD";
        default:
            return "ERROR";
    }
}

