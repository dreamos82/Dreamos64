#include <task.h>
#include <scheduler.h>
#include <kheap.h>
#include <string.h>
#include <logging.h>
#include <vm.h>
#include <kernel.h>
#include <vmm.h>
#include <pmm.h>

extern uint64_t p4_table[];
extern uint64_t p3_table[];
extern uint64_t p3_table_hh[];

task_t* create_task(char *name, void (*_entry_point)(void *), void *args) {
    //disable interrupts while creating a task
    asm("cli");
    task_t* new_task = (task_t*) kmalloc(sizeof(task_t));
    strcpy(new_task->task_name, name);
    new_task->parent = NULL;
    new_task->task_id = next_task_id++;
    loglinef(Verbose, "(create_task) Task created with name: %s - Task id: %d", new_task->task_name, new_task->task_id);
    if( _entry_point != NULL) {
        thread_t* thread = create_thread(name, _entry_point, args, new_task);
        new_task->threads = thread;
    }
    prepare_virtual_memory_environment(new_task);
    scheduler_add_task(new_task);
    //load_cr3(new_task->vm_root_page_table);
    //re-enable interrupts
    asm("sti");
    return new_task;
}

void prepare_virtual_memory_environment(task_t* task) {
    loglinef(Verbose, "(prepare_virtual_memory_environment) Placeholder for virtual_memory");
    // Steps:
    // 1. Prepare resources: allocatin an array of VM_PAGES_PER_TABLE
    // Make sure this address is physical, then it needs to be mapped to a virtual one.a
    task->vm_root_page_table = pmm_alloc_frame();
    loglinef(Verbose, "(prepare_virtual_memory_environment) vm_root_page_table address: %x", task->vm_root_page_table);
    // This part will be changed using vmm_alloc
    //identity_map_phys_address(task->vm_root_page_table, 0);
    // I will get the page frame first, then get virtual address to map it to with vmm_alloc, and then do the mapping on the virtual address.
    
    uintptr_t  vm_root_vaddress = vmm_alloc(PAGE_SIZE_IN_BYTES, VMM_FLAGS_ADDRESS_ONLY);
    map_phys_to_virt_addr(task->vm_root_page_table, vm_root_vaddress, VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE);

    // 2. We will map the whole higher half of the kernel, this means from pml4 item 256 to 511
    //    ((uint64_t *)task->vm_root_page_table)[0] = p4_table[0];
    for(int i = 255; i < VM_PAGES_PER_TABLE; i++) {
        ((uint64_t *)vm_root_vaddress)[i] = p4_table[i];
        if(p4_table[i] != 0) {
            loglinef(Verbose, "(prepare_virtual_memory_environment): %d: o:0x%x - c:0x%x - t:0x%x", i, p4_table[i], kernel_settings.paging.page_root_address[i], ((uint64_t*)vm_root_vaddress)[i]);
            
        }
    }
}

bool add_thread_to_task_by_id(size_t task_id, thread_t* thread) {
    task_t* task = get_task(task_id);
    if (task == NULL) {
        return false;
    }
    thread->parent_task = task;
    thread->next = task->threads;
    thread->next_sibling = thread;
    return true;
}

bool add_thread_to_task(task_t* task, thread_t* thread) {
    if (task == NULL || thread == NULL) {
        return false;
    }
    thread->next = task->threads;
    thread->next_sibling = thread;
    return true;
}

task_t* get_task(size_t task_id) {
    if (task_id > next_task_id) {
        return NULL;
    }
    task_t* cur_task = root_task;
    while ( cur_task != NULL ) {
        loglinef(Verbose, "(get_task) Searching task: %d", cur_task->task_id);
        if ( cur_task->task_id == task_id ) {
            return cur_task;
        }
        cur_task = cur_task->next;
    }
    return NULL;
}

void print_thread_list(size_t task_id) {
    task_t* task = get_task(task_id);
    if (task != NULL) {
        thread_t* thread = task->threads;
        while(thread != NULL) {
            loglinef(Verbose, "(print_thread_list)\tThread; %d - %s", thread->tid, thread->thread_name);
            thread = thread->next;
        }
    }
}
