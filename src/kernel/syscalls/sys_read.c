#include <devices.h>
#include <kheap.h>
#include <logging.h>
#include <ps2_keyboard_driver.h>
#include <scheduler.h>
#include <sys_read.h>
#include <task.h>
#include <vfs.h>
#include <vmm_mapping.h>

bool buffer_setup = false;

ssize_t sys_read(int fildes, void *buf, size_t nbytes) {
    if (fildes >= 0 && fildes < OPENEDFILES_MAX) {
        int counter = 0;
        if(fildes == 0) {
            return sys_read_keyboard( buf, nbytes );
        } else {
            /*if (vfs_opened_files[fildes].fs_specific_id >= 0) {
            int fs_specific_id = vfs_opened_files[fildes].fs_specific_id;
            int mountpoint_id = vfs_opened_files[fildes].mountpoint_id;
            mountpoint_t mountpoint = mountpoints[mountpoint_id];
            if (mountpoint.file_operations.read != NULL) {
                return mountpoint.file_operations.read(fs_specific_id, buf, nbytes);
            }*/
            return 0;
        }
    }
    return -1;
}


ssize_t sys_read_keyboard(void *buffer, size_t nbytes) {
    // Buffer address is relative to the process.
    // I need to get access to the current task and get the buffer address inside the hhdm
    // To let the kernel access it's content
    // This information is stored inside the vmm_data field in the task structrure
    task_t *current_task = current_executing_thread->parent_task;
    VmmInfo vmm_info = current_task->vmm_data;
    pending_operation_t *new_pending_operation = kmalloc(sizeof(pending_operation_t));
    userspace_buffer_t *userspace_buffer = kmalloc(sizeof(userspace_buffer_t));
    pretty_logf(Verbose, "nbytes to read: %d", nbytes);
    if (buffer_setup == false) {
        // I need to create a userspace_buffer_t item
        // This structure will be used to contain the information about the buffer in the
        userspace_buffer->info = vmm_info;
        userspace_buffer->buffer_base = (uintptr_t) buffer;
        userspace_buffer->length = nbytes;
        userspace_buffer->buffer_virtual = vm_copy_from_different_space( (uintptr_t) buffer, (uint64_t*) vmm_info.root_table_hhdm);
    }
    if (userspace_buffer->buffer_virtual == NULL) {
        pretty_log(Verbose, "Cannot convert given address");
        return 0;
    }
    if (nbytes > 0 && buffer_setup == false) {
        new_pending_operation->buffer = userspace_buffer;
        new_pending_operation->nbytes = 0;
        new_pending_operation->read = false;
        new_pending_operation->next = NULL;
        _ps2_keyboard_add_operation(new_pending_operation);
        buffer_setup = true;
    }


    while(new_pending_operation->nbytes < nbytes) {
        scheduler_yield();
    }
    return 1;
}
