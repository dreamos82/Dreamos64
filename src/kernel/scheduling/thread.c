#include <thread.h>
#include <scheduler.h>
#include <stdio.h>
#include <video.h>
#include <string.h>

thread_t create_thread(char* thread_name) {
    thread_t new_thread;
    //new_thread.tid = next_thread_id++;  
    new_thread.tid = 1;
    //strcpy(new_thread.thread_name, thread_name);
    printf("Creating a new_thread with name: %s\n", thread_name);
    return new_thread;
}
