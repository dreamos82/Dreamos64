#include <devices.h>
#include <kheap.h>
#include <logging.h>
#include <scheduler.h>
#include <sys_read.h>
#include <task.h>
#include <vfs.h>
#include <vmm_mapping.h>

pending_operation_t cur_operation;

ssize_t sys_read(int fildes, void *buf, size_t nbytes) {
    if (fildes >= 0 && fildes < OPENEDFILES_MAX) {
        int counter = 0;
        if(fildes == 0) {
            pretty_log(Verbose, "FD for stdin");
        }
        if (vfs_opened_files[fildes].fs_specific_id >= 0) {
            int fs_specific_id = vfs_opened_files[fildes].fs_specific_id;
            int mountpoint_id = vfs_opened_files[fildes].mountpoint_id;
            mountpoint_t mountpoint = mountpoints[mountpoint_id];
            if (mountpoint.file_operations.read != NULL) {
                return mountpoint.file_operations.read(fs_specific_id, buf, nbytes);
            }
            return 0;
        }
    }
    return -1;
}


ssize_t sys_read_keyboard(void *buffer, size_t nbytes) {
    // Buffer address is relative to the process.
    // I need to get access to the current task
    task_t *current_task = current_executing_thread->parent_task;
    VmmInfo vmm_info = current_task->vmm_data;
    // I need to create a userspace_buffer_t item
    userspace_buffer_t userspace_buffer;
    userspace_buffer.info = vmm_info;
    userspace_buffer.buffer_base = (uintptr_t) buffer;
    userspace_buffer.buffer_virtual = vm_copy_from_different_space( (uintptr_t) buffer, (uint64_t*) vmm_info.root_table_hhdm);
    if (userspace_buffer.buffer_virtual == NULL) {
        pretty_log(Verbose, "Cannot convert given address");
        return 0;
    } else {
        pretty_logf(Verbose, "Returned address: 0x%x", userspace_buffer.buffer_virtual);
    }
    if (nbytes > 0) {
        cur_operation.buffer = userspace_buffer;
        cur_operation.nbytes = nbytes;
        cur_operation.read = false;
        cur_operation.next = NULL;
        _ps2_keyboard_add_operation(&cur_operation)
    }
    // I need to allocate a ps2_op
    //pending_operation_t *ps2_op = (pending_operation_t *) kmalloc(sizeof(pending_operation_t));
    return 1;
}
