#ifndef _TASK_H
#define _TASK_H

#include <devices.h>
#include <elf.h>
#include <stddef.h>
#include <stdbool.h>
#include <thread.h>
#include <vmm.h>

#define TASK_NAME_MAX_LEN 32

#define DRMOS_MAX_FILE_DESCRIPTORS 64

typedef struct task_t task_t;

struct task_t {
    size_t task_id;
    char task_name[TASK_NAME_MAX_LEN];

    // It will contain the virtual memory base address for the process
    void* vm_root_page_table;

    VmmInfo vmm_data;

    file_descriptor_t file_descriptors[DRMOS_MAX_FILE_DESCRIPTORS];

    //List of threads
    struct thread_t* threads;
    task_t* parent;
    task_t* next;
};

extern size_t next_task_id;

task_t* create_task( char *name, bool is_supervisor );
task_t *create_task_from_func(char *name, void (*_entry_point)(void *), void *args, bool is_supervisor);
task_t *create_task_from_elf(char *name, void *args, Elf64_Ehdr *elf_header);

task_t* get_task( size_t task_id );

bool add_thread_to_task_by_id( size_t task_id, thread_t* thread );
bool add_thread_to_task( task_t* task, thread_t* thread );

bool delete_thread_from_task( size_t thread_id, task_t *task );

void prepare_virtual_memory_environment(task_t* task);

void print_thread_list( size_t task_id );

bool remove_thread_from_task(size_t thread_id, task_t *task);

#endif
