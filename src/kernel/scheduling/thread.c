#include <thread.h>
#include <scheduler.h>
#include <stdio.h>
#include <video.h>
#include <string.h>
#include <kheap.h>

thread_t* create_thread(char* thread_name) {
    thread_t *new_thread = kmalloc(sizeof(thread_t));
    new_thread->tid = next_thread_id++;
    new_thread->status = INIT;
    strcpy(new_thread->thread_name, thread_name);
    //strcpy(new_thread.thread_name, thread_name);
    //TODO Intialize cpu_status_t
    return new_thread;
}
