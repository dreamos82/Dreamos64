#include <elf.h>
#include <hh_direct_map.h>
#include <kernel.h>
#include <kheap.h>
#include <logging.h>
#include <vm.h>
#include <pmm.h>
#include <scheduler.h>
#include <task.h>
#include <string.h>
#include <vmm.h>
#include <vmm_mapping.h>
#include <vmm_util.h>

extern uint64_t p4_table[];
extern uint64_t p3_table[];
extern uint64_t p3_table_hh[];

task_t* create_task(char *name, bool is_supervisor) {
    //disable interrupts while creating a task
    task_t* new_task = (task_t*) kmalloc(sizeof(task_t));
    strcpy(new_task->task_name, name);
    new_task->parent = NULL;
    new_task->task_id = next_task_id++;
    pretty_logf(Verbose, "Task created with name: %s - Task id: %d", new_task->task_name, new_task->task_id);
    //prepare_virtual_memory_environment(new_task);
    if ( is_supervisor ){
        vmm_init(VMM_LEVEL_SUPERVISOR, &(new_task->vmm_data));
    } else {
        vmm_init(VMM_LEVEL_USER, &(new_task->vmm_data));
    }
    //scheduler_add_task(new_task);
    //re-enable interrupts
    return new_task;
}

task_t *create_task_from_elf(char *name, void *args, Elf64_Ehdr *elf_header){
    asm("cli");
    // I will not create a task from elf if is_supervisor is true.
    task_t* new_task = create_task(name,  false);
    prepare_virtual_memory_environment(new_task);
    if(elf_header != NULL) {
        //Here i will put the code to handle the case where an Elf is passed.
        Elf64_Half phdr_entries = elf_header->e_phnum;
        Elf64_Half phdr_entsize = elf_header->e_phentsize;
        pretty_logf(Verbose, " Number of PHDR entries: 0x%x", phdr_entries);
        pretty_logf(Verbose, " PHDR Entry Size: 0x%x", phdr_entsize );
        pretty_logf(Verbose, " ELF Entry point: 0x%x", elf_header->e_entry);
        Elf64_Phdr *phdr_list = (Elf64_Phdr*) ((uintptr_t) elf_header + elf_header->e_phoff);
        for ( int i = 0; i < phdr_entries; i++) {
            // I need first to compute the number of pages required for each phdr
            // clear all the memory not used
            // compute the entries for each page and insert them into the page tables.
            Elf64_Phdr phdr = phdr_list[i];
            size_t vmm_hdr_flags = elf_flags_to_memory_flags(phdr.p_flags);
            pretty_logf(Verbose, "\t[%d]: Type: 0x%x, Flags: 0x%x  -  Vaddr: 0x%x - aligned: 0x%x ", i, phdr.p_type, phdr.p_flags, phdr.p_vaddr, align_value_to_page(phdr.p_vaddr));
            pretty_logf(Verbose, "\t\t - FileSz: 0x%x, Memsz: 0x%x, vmm flags: 0x%x - p_align: 0x%x - p_offset: 0x%x", phdr.p_filesz, phdr.p_memsz, vmm_hdr_flags, phdr.p_align, phdr.p_offset);
            Elf64_Half mem_pages = (Elf64_Half) get_number_of_pages_from_size(phdr.p_memsz);
            Elf64_Half filesz_pages = (Elf64_Half) get_number_of_pages_from_size(phdr.p_filesz);
            uint64_t offset_address = (uint64_t) ((uint64_t) elf_header + (uint64_t) phdr.p_offset);
            uint64_t vaddr_address = align_value_to_page(phdr.p_vaddr);
            for (int j = 0; j < mem_pages; j++) {
                pretty_logf(Verbose, "[%d]: Mapping: offset: 0x%x (virtual: 0x%x) in vaddr: 0x%x", j, hhdm_get_phys_address((uintptr_t) offset_address), offset_address, vaddr_address);
                if ( !is_address_aligned((size_t) hhdm_get_phys_address(offset_address), PAGE_SIZE_IN_BYTES)) {
                    pretty_log(Fatal, "Error: module elf phys address is not page aligned");
                }

                map_phys_to_virt_addr_hh(hhdm_get_phys_address(offset_address), (void *) vaddr_address, VMM_FLAGS_USER_LEVEL | vmm_hdr_flags | VMM_FLAGS_PRESENT, (uint64_t *) new_task->vmm_data.root_table_hhdm);
                //I need a mem copy. I need to fopy the content of elf_header + phdr.p_offset into phdr.p_vaddr
                offset_address += (uint64_t) phdr.p_align;
                vaddr_address += (uint64_t) phdr.p_align;
                //I need to allocate memory and map it into the new memory space,
            }
        }
        thread_t* thread = create_thread(name, (void (*)(void *))elf_header->e_entry, args, new_task, false, true);
        pretty_logf(Verbose, "Thread created: id: %d - Entry Point: 0x%x - elf header entry point: 0x%x", thread->tid, thread->execution_frame->rip, elf_header->e_entry);
    }
    // Create a new thread
    scheduler_add_task(new_task);
    asm("sti");
    return new_task;
}

task_t *create_task_from_func(char *name, void (*_entry_point)(void *), void *args, bool is_supervisor) {
    asm("cli");
    task_t* new_task = create_task(name,  is_supervisor);
    prepare_virtual_memory_environment(new_task);
        if( is_supervisor) {
        pretty_logf(Verbose, "creating new supervisor thread: %s", name);
        thread_t* thread = create_thread(name, _entry_point, args, new_task, is_supervisor, false);
        new_task->threads = thread;
    } else {
        pretty_logf(Verbose, "creating new userspace thread %s", name);
        thread_t* thread = create_thread(name, NULL, args, new_task, is_supervisor, false);
        new_task->threads = thread;
    }
    scheduler_add_task(new_task);
    asm("sti");
}

void prepare_virtual_memory_environment(task_t* task) {
    // Steps:
    // 1. Prepare resources: allocatin an array of VM_PAGES_PER_TABLE
    // Make sure this address is physical, then it needs to be mapped to a virtual one.a
    task->vm_root_page_table = pmm_alloc_frame();
    //pretty_logf(Verbose, "vm_root_page_table address: %x", task->vm_root_page_table);
    //identity_map_phys_address(task->vm_root_page_table, 0);
    // I will get the page frame first, then get virtual address to map it to with vmm_alloc, and then do the mapping on the virtual address.
    // Technically the vmm_alloc is not needed, since i have the direct memory map already accessible, so i just need to access it through the direct m
    //void* vm_root_vaddress = vmm_alloc(PAGE_SIZE_IN_BYTES, VMM_FLAGS_ADDRESS_ONLY, NULL);
    void* vm_root_vaddress = hhdm_get_variable ((uintptr_t) task->vm_root_page_table);
    task->vmm_data.root_table_hhdm = (uintptr_t) vm_root_vaddress;
    pretty_logf(Verbose, "vm_root_vaddress: %x", vm_root_vaddress);
    //map_phys_to_virt_addr(task->vm_root_page_table, vm_root_vaddress, VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE, NULL);

    // 2. We will map the whole higher half of the kernel, this means from pml4 item 256 to 511
    //    ((uint64_t *)task->vm_root_page_table)[0] = p4_table[0];
    for(int i = 0; i < VM_PAGES_PER_TABLE; i++) {
        if (i <=255) {
            ((uint64_t *)vm_root_vaddress)[i] = 0x00;
        } else if ( i == 510 ) {
            ((uint64_t *)vm_root_vaddress)[i] = (uint64_t) (task->vm_root_page_table) | VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE;
            pretty_logf(Verbose, "Mapping recursive entry: 0x%x", ((uint64_t *)vm_root_vaddress)[i]);
        } else {
            ((uint64_t *)vm_root_vaddress)[i] = p4_table[i];
        }
        if (p4_table[i] != 0) {
            pretty_logf(Verbose, "\t%d: o:0x%x - c:0x%x - t:0x%x", i, p4_table[i], kernel_settings.paging.page_root_address[i], ((uint64_t*)vm_root_vaddress)[i]);
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
    pretty_logf( Verbose, "Removing thread with thread id: %d, from task: %d with name: %s", thread_id, task->task_id, task->task_name);
    while ( cur_thread != NULL ) {
        if ( cur_thread->tid == thread_id ) {
            pretty_logf( Verbose, "Found thread to remove thread name: %s", cur_thread->thread_name);
            if ( cur_thread == task->threads) {
                pretty_logf( Verbose, "Is the first thread in the queue addr_value: 0x%x", cur_thread->next_sibling);
                task->threads = cur_thread->next_sibling;
                return true;;
            } else {
                pretty_log( Verbose, "Is in the middle, merging prev and cur->next_sibling");
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
        pretty_logf(Verbose, "Searching task: %d", cur_task->task_id);
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
            pretty_logf(Verbose, "\tThread; %d - %s", thread->tid, thread->thread_name);
            thread = thread->next;
        }
    }
}
