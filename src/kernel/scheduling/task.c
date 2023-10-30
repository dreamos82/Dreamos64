#include <hh_direct_map.h>
#include <task.h>
#include <scheduler.h>
#include <kheap.h>
#include <string.h>
#include <logging.h>
#include <vm.h>
#include <kernel.h>
#include <vmm.h>
#include <vmm_mapping.h>
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
    prepare_virtual_memory_environment(new_task);
    vmm_init(VMM_LEVEL_USER, &(new_task->vmm_data));
    if( _entry_point != NULL) {
        thread_t* thread = create_thread(name, _entry_point, args, new_task);
        new_task->threads = thread;
    }
    scheduler_add_task(new_task);
    //re-enable interrupts
    asm("sti");
    return new_task;
}

void prepare_virtual_memory_environment(task_t* task) {
    // Steps:
    // 1. Prepare resources: allocatin an array of VM_PAGES_PER_TABLE
    // Make sure this address is physical, then it needs to be mapped to a virtual one.a
    task->vm_root_page_table = pmm_alloc_frame();
    loglinef(Verbose, "(%s) vm_root_page_table address: %x", __FUNCTION__, task->vm_root_page_table);
    //identity_map_phys_address(task->vm_root_page_table, 0);
    // I will get the page frame first, then get virtual address to map it to with vmm_alloc, and then do the mapping on the virtual address.
    // Tecnically the vmm_allos is not needed, since i have the direct memory map already accessible, so i just need to access it through the direct map.

    //void* vm_root_vaddress = vmm_alloc(PAGE_SIZE_IN_BYTES, VMM_FLAGS_ADDRESS_ONLY, NULL);
    void* vm_root_vaddress = hhdm_get_variable ((size_t) task->vm_root_page_table);
    //map_phys_to_virt_addr(task->vm_root_page_table, vm_root_vaddress, VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE, NULL);

    // 2. We will map the whole higher half of the kernel, this means from pml4 item 256 to 511
    //    ((uint64_t *)task->vm_root_page_table)[0] = p4_table[0];
    for(int i = 0; i < VM_PAGES_PER_TABLE; i++) {
        if (i <=255) {
            ((uint64_t *)vm_root_vaddress)[i] = 0x00;
        } else if ( i == 510 ) {
            ((uint64_t *)vm_root_vaddress)[i] = (uint64_t) (task->vm_root_page_table) | VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE;
            loglinef(Verbose, "(%s): Mapping recursive entry: 0x%x", __FUNCTION__, ((uint64_t *)vm_root_vaddress)[i]);
        } else {
            ((uint64_t *)vm_root_vaddress)[i] = p4_table[i];
        }
        if (p4_table[i] != 0) {
            loglinef(Verbose, "(prepare_virtual_memory_environment): %d: o:0x%x - c:0x%x - t:0x%x", i, p4_table[i], kernel_settings.paging.page_root_address[i], ((uint64_t*)vm_root_vaddress)[i]);
        }
    }
}

bool add_thread_to_task_by_id( size_t task_id, thread_t* thread ) {
    task_t* task = get_task(task_id);
    if (task == NULL) {
        return false;
    }
    thread->parent_task = task;
    thread->next = task->threads;
    thread->next_sibling = thread;
    return true;
}

bool remove_thread_from_task(size_t thread_id, task_t *task) {
    // We don't freethe thread here, because is the scheduler in charge of deleting DEAD threads
    thread_t *cur_thread = task->threads;
    thread_t *prev_thread = cur_thread;
    loglinef( Verbose, "(%s) Removing thread with thread id: %d, from task: %d with name: %s", __FUNCTION__, thread_id, task->task_id, task->task_name);
    while ( cur_thread != NULL ) {
        if ( cur_thread->tid == thread_id ) {
            loglinef( Verbose, "(%s) Found thread to remove thread name: %s", __FUNCTION__, cur_thread->thread_name);
            if ( cur_thread == task->threads) {
                loglinef( Verbose, "(%s), Is the first thread in the queue addr_value: 0x%x", __FUNCTION__, cur_thread->next_sibling);
                task->threads = cur_thread->next_sibling;
                return true;;
            } else {
                loglinef( Verbose, "(%s), Is in the middle, mergin prev and cur->next_sibling", __FUNCTION__);
                prev_thread->next_sibling = cur_thread->next_sibling;
            }
        }
        prev_thread = cur_thread;
        cur_thread = cur_thread->next_sibling;
        return true;
    }
    return false;
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
