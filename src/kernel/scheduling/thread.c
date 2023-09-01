#include <thread.h>
#include <scheduler.h>
#include <stdio.h>
#include <framebuffer.h>
#include <string.h>
#include <kheap.h>
#include <logging.h>
#include <kernel.h>
#include <vmm.h>

thread_t* create_thread(char* thread_name, void (*_entry_point)(void *), void* arg, task_t* parent_task) {
    thread_t *new_thread = kmalloc(sizeof(thread_t));
    new_thread->tid = next_thread_id++;
    new_thread->parent_task = parent_task;
    new_thread->status = NEW;
    new_thread->wakeup_time = 0;
    strcpy(new_thread->thread_name, thread_name);
    new_thread->next = NULL;
    new_thread->next_sibling = NULL;
    new_thread->ticks = 0;
    loglinef(Verbose, "(create_thread): Creating thread with arg: %c - arg: %x - name: %s - rip: %u", (char) *((char*) arg), arg, thread_name, _entry_point);

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
    uintptr_t stack_pointer = (uintptr_t) kmalloc(THREAD_DEFAULT_STACK_SIZE);
    if(stack_pointer == NULL) {
        loglinef(Fatal, "(create_thread): rsp is null - PANIC!");
        while(1);
    }

    // The stack grow backward, so the pointer will be the end of the stack
    new_thread->stack = stack_pointer + THREAD_DEFAULT_STACK_SIZE;
    new_thread->execution_frame->rsp = (uint64_t) new_thread->stack;
    new_thread->execution_frame->rbp = 0;

    if (parent_task != NULL) {
        add_thread_to_task(parent_task, new_thread);
    } else {
        loglinef(Fatal, "(%s): Cannot create thread without parent task");
    }

    scheduler_add_thread(new_thread);
    return new_thread;
}

void thread_sleep(size_t millis) {
    current_executing_thread->status = SLEEP;
    uint64_t kernel_uptime = get_kernel_uptime();
    current_executing_thread->wakeup_time = kernel_uptime + millis; // To change with millis since boot + millis
    loglinef(Verbose, "(thread_sleep) Kernel uptime is: %u - wakeup time is: %u", kernel_uptime, current_executing_thread->wakeup_time);
    scheduler_yield();
}

void thread_wakeup(thread_t* thread) {

}

void thread_suicide_trap() {
    current_executing_thread->status = DEAD;
    loglinef(Verbose, "(thread_suicide_trap) Suicide function called on thread: %d name: %s - Status: %s", current_executing_thread->tid, current_executing_thread->thread_name, get_thread_status(current_executing_thread));
    while(1);
}

void thread_execution_wrapper( void (*_thread_function)(void *), void* arg) {
    _thread_function(arg);
    thread_suicide_trap();
    return;
}

void idle() {
    while(1);
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
        i++;
        //loglinef(Verbose, "Task2: %c - %d", (char) *c, i);
        #if USE_FRAMEBUFFER == 1
        _fb_printStr(str, 0, 12, 0x000000, 0xE169CD);
        #endif

    }
}

void noop3(char *c) {
    int i=0;
    char str[4];
    str[0] = (char) *c;
    str[1] = 'b';
    str[2] = 'f';
    str[3] = '\0';
    while(i < 10000) {
        i++;
        //loglinef(Verbose, "Task2: %c - %d", (char) *c, i);
        #if USE_FRAMEBUFFER == 1
        _fb_printStr(str, 0, 12, 0x000000, 0xE169CD);
        #endif
    }
    loglinef(Verbose, "(noop3) Going to sleep %d", get_kernel_uptime());
    thread_sleep(5000);
    loglinef(Verbose, "(test_task noop3): Wakeup %d - %d", get_kernel_uptime(), current_executing_thread->wakeup_time);
    i = 0;
    while(i < 1000) {
        i++;
        //loglinef(Verbose, "Task2: r- %d", (char) *c, i);
        #if USE_FRAMEBUFFER == 1
        _fb_printStr("r", 1, 12, 0x000000, 0xE169CD);
        #endif

    }

    loglinef(Verbose, "(%s): allocating 100 bytes", __FUNCTION__);
    vmm_alloc(100, VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE, NULL);
    loglinef(Verbose, "(%s): end allocating 100 bytes", __FUNCTION__);
    uint64_t *test_addr = (uint64_t  *) vmm_alloc(2097253, 0, NULL);
    test_addr[0] = 5;
    loglinef(Verbose, "(noop3): test_addr[0] = %d", test_addr[0]);
    task_t *current_task = current_executing_thread->parent_task;
    uint64_t *tmp_var = vmm_alloc(0x1000, VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE, &(current_task->vmm_data));
    loglinef(Verbose, "(%s) task name: %s - Tmp var address returned by vmm_alloc: 0x%x", __FUNCTION__, current_task->task_name, tmp_var);
    tmp_var[0] = 0x1234;
    loglinef(Verbose, "(%s) Tmp var address returned by vmm_alloc: 0x%x==0x1234 ", __FUNCTION__, tmp_var[0]);
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

